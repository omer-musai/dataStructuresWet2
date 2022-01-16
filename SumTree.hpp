#ifndef AVLTree_HPP
#define AVLTree_HPP

#define AVL_BALANCE_BOUND 1

#include "SumTreeNode.hpp"
#include "game_exceptions.hpp"

#include <cassert>
#include <memory>

class SumTree
{
private:
    int levelZero;
    SumTreeNode *root;
    SumTreeNode *highest;
    int nodeCount;

    //Static utilities: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    class StaticAVLUtilities
    {
        friend class SumTree;
        class ArrayFromTreePopulator {
        private:
            int* array;
            int* levels;
            int size;
            int index;
            bool reverse;

        public:
            ArrayFromTreePopulator() = delete;
            explicit ArrayFromTreePopulator(int* array, int* levels, int size = -1, bool reverse=false):
                    array(array), levels(levels), size(size), index(0), reverse(reverse)
            {
                if (reverse)
                {
                    if (size <= 0)
                    {
                        throw InvalidInput("Populator: reversing is only possible if size is provided.");
                    }
                    index = size - 1;
                }
            }

            void operator()(int level, int inThisLevel, int height, int parentValue, int leftValue, int rightValue, int BF) {
                if (reverse)
                {
                    if(size < 0 || index >= 0)
                    {
                        array[index] = level;
                        levels[index--] = inThisLevel;
                    }
                }
                else
                {
                    if (size < 0 || index < size)
                    {
                        array[index] = level;
                        levels[index++] = inThisLevel;
                    }
                }
            }
        };

        static int* treeToArray(const SumTree& tree, int** levels, bool reverse=false) {
            int* array = new int[tree.getSize()];
            *levels = new int[tree.getSize()];
            try
            {
                ArrayFromTreePopulator populator(array, *levels, tree.getSize(), reverse);
                tree.inorder(populator);
            }
            catch (std::exception& exception)
            {
                delete[] array;
                delete[] *levels;
                throw exception;
            }
            return array;
        }

        static int* arrayMerge(int* arr1, int* levels1, int size1, int* arr2, int* levels2, int size2, int **levelsMerged, int *length) {
            assert(levelsMerged != nullptr && length != nullptr);
            int* array = new int[size1 + size2];
            *levelsMerged = new int[size1 + size2]; //Might take more space than needed. Could realloc in the end if it matters.
            try
            {
                int i1 = 0, i2 = 0, i = 0;
                while (i1 < size1 && i2 < size2) {
                    if (arr1[i1] == arr2[i2])
                    {
                        array[i] = arr1[i1];
                        (*levelsMerged)[i] = levels1[i1++] + levels2[i2++];
                    }
                    else if (arr1[i1] < arr2[i2])
                    {
                        array[i] = arr1[i1];
                        (*levelsMerged)[i] = levels1[i1++];
                    }
                    else
                    {
                        array[i] = arr2[i2];
                        (*levelsMerged)[i] = levels2[i2++];
                    }
                    ++i;
                }
                while (i1 < size1) {
                    array[i] = arr1[i1];
                    (*levelsMerged)[i++] = levels1[i1++];
                }
                while (i2 < size2) {
                    array[i] = arr2[i2];
                    (*levelsMerged)[i++] = levels2[i2++];
                }

                *length = i;
            }
            catch (std::out_of_range& exception)
            {
                delete[] array;
                delete[] *levelsMerged;
            }

            return array;
        };

        //This uses the algorithm described & proved in the doc.
        static std::unique_ptr<SumTree> AVLFromArray(int* arr, int* inThisLevel, int size) {
            assert(size >= 0);

            std::unique_ptr<SumTree> tree = std::unique_ptr<SumTree>(new SumTree());
            if (size > 0)
            {
                int m = (size % 2 == 0 ? size / 2 : (size + 1) / 2) - 1; //m=ceil(size/2)-1
                tree->addNode(arr[m], inThisLevel[m]); //O(1) since the tree is empty.

                if (size == 2) {
                    tree->addNode(arr[m + 1], inThisLevel[m + 1]); //O(1) since the tree is always sized 1 and this point.
                } else if (size > 2) {
                    tree->setLeftSubtree(AVLFromArray(arr, inThisLevel, m));
                    tree->setRightSubtree(
                            AVLFromArray(arr + (m + 1), inThisLevel + (m + 1), size - m - 1)
                        );
                }
            }

            return tree;
        }

        //THIS RUINS THE PARAMETER TREES. Careful!
        static std::unique_ptr<SumTree> mergeTrees(SumTree& t1, SumTree& t2) {
            int *t1arr = nullptr, *t2arr = nullptr, *merged = nullptr,
                *t1levels = nullptr, *t2levels = nullptr, *levelsMerged = nullptr, size;
            int levelZero = t1.levelZero + t2.levelZero;
            std::unique_ptr<SumTree> result;
            try {
                t1arr = treeToArray(t1, &t1levels);
                t2arr = treeToArray(t2, &t2levels);
                merged = arrayMerge(t1arr, t1levels, t1.getSize(), t2arr, t2levels, t2.getSize(), &levelsMerged, &size);
                result = treeFromArray(merged, levelsMerged, size);
                result->levelZero = levelZero;
                t1.clean();
                t2.clean();
            }
            catch (std::exception &exception) {

            }

            delete[] t1arr;
            delete[] t2arr;
            delete[] merged;
            delete[] levelsMerged;
            delete[] t1levels;
            delete[] t2levels;

            return result;
        }
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    };


    enum Order { smaller, equal, larger };

    static int abs(int a) { return a > 0 ? a : -a; }

    void setLeftSubtree(const std::shared_ptr<SumTree>& tree)
    {
        assert(root != nullptr);
        root->setLeft(tree->root);
        tree->root = nullptr; //To ensure no scary O(n) cleaning operations take place.
        root->updateHeight();
        nodeCount = tree->nodeCount + nodeCount;
    }

    void setRightSubtree(const std::shared_ptr<SumTree>& tree)
    {
        assert(root != nullptr);
        root->setRight(tree->root);
        tree->root = nullptr; //To ensure no scary O(n) cleaning operations take place.
        if (tree->highest != nullptr)
        {
            this->highest = tree->highest;
        }
        root->updateHeight();
        nodeCount = tree->nodeCount + nodeCount;
    }

    void freeList()
    {
        freeListAux(root);
    }

    void freeListAux(SumTreeNode* curr)
    {
        if (curr == nullptr) return;
        freeListAux(curr->getLeft());
        freeListAux(curr->getRight());
        delete curr;
    }

    SumTreeNode* findLocation(int level, Order &orderRel)
    {
        if (root == nullptr) return nullptr;
        return findLocationAux(level, root, orderRel);
    }

    SumTreeNode* findLocationAux(int level, SumTreeNode* curr, Order &orderRel)
    {
        if (level == curr->getLevel())
        {
            orderRel = equal;
            return curr;
        }

        bool isLarger = level > curr->getLevel();
        SumTreeNode* nextNode = isLarger ? curr->getRight() : curr->getLeft();

        if (nextNode == nullptr)
        {
            orderRel = isLarger ? larger : smaller;
            return curr;
        }

        return findLocationAux(level, nextNode, orderRel);
    }

    /*
     * Like findLocation, but returns the next node in order and the prev node in order IF the node
     * was not found (this is done via the before and after parameters).
     * If the node was found, the return value AS WELL AS BEFORE & AFTER will point to it.
     * If the node was not found, the return value will nullptr, and before & after will point to the
     * next & prev in order.
     *
     * Before and after can be provided as nullptr, in which case they will be ignored.
     * (Hence, having both of them as nullptr effectively mimics findLocation in all respects.)
     */
    SumTreeNode* findLocationBounds(int level, Order &orderRel, SumTreeNode** before=nullptr, SumTreeNode** after=nullptr) const
    {
        if (before != nullptr) *before = nullptr;
        if (after != nullptr) *after = nullptr;
        if (root == nullptr)
        {
            return nullptr;
        }
        return findLocationBoundsAux(level, root, orderRel, before, after);
    }

    SumTreeNode* findLocationBoundsAux(int level, SumTreeNode* curr, Order &orderRel, SumTreeNode** before, SumTreeNode** after) const
    {
        if (level == curr->getLevel())
        {
            orderRel = equal;
            if (after != nullptr)
            {
                *after = curr;
            }
            if (before != nullptr)
            {
                *before = curr;
            }
            return curr;
        }

        bool isLarger = level > curr->getLevel();
        SumTreeNode* nextNode;

        if (isLarger)
        {
            nextNode = curr->getRight();
            if (before != nullptr) *before = curr;
        }
        else
        {
            nextNode = curr->getLeft();
            if (after != nullptr) *after = curr;
        }

        if (nextNode == nullptr)
        {
            orderRel = isLarger ? larger : smaller;
            return curr;
        }

        return findLocationBoundsAux(level, nextNode, orderRel, before, after);
    }

    /*
     * The lowerThan described in the LyX document under getPercentOfPlayersWithScoreInBounds.
     * It'd be more efficient to merge this with findLocationBounds, but asymptotically it wouldn't
     * matter and we're a bit short on time.
     * If node == nullptr, return all players in tree except level zeroes.
     * NOTE: THIS SHOULD ONLY BE CALLED ON NODES THAT ARE CONFIRMED TO BE IN THE TREE.
     * (That's while it takes a node despite only needing a level.)
     */
    int lowerThan(const SumTreeNode* node, bool larger=true) const
    {
        if (node == nullptr)
        {
            if (larger)
            {
                return root == nullptr ? 0 : root->getW();
            }
            return 0;
        }
        int level = node->getLevel();
        int r = 0;
        SumTreeNode* curr = root;
        while (curr != nullptr)
        {
            if (level == curr->getLevel())
            {
                return r + curr->getLeftW();
            }
            if (level > curr->getLevel())
            {
                r += curr->getLeftW() + curr->getInThisLevel();
                curr = curr->getRight();
            }
            else
            {
                curr = curr->getLeft();
            }
        }

        assert(false); //shouldn't get here
        return -1;
    }

    /*template <class K>
    SumTreeNode* findByKey(K key, bool* found)
    {
        int value(key);
        Order orderRel;
        SumTreeNode* node = findLocation(value, orderRel);
        if (found != nullptr)
        {
            *found = !(node == nullptr || orderRel != equal);
        }
        return node;
    }*/

    //TODO: remove the vast commented-out portions once certain they're not needed.

    //Updates heights and returns the lowest node with an invalid balance factor.
    void updateTree(SumTreeNode* node)
    {
        if (node == nullptr) return;

        node->updateHeight();
        SumTreeNode* nextNode = rotate(node);

        SumTreeNode* parent = nextNode->getParent();

        if (parent == nullptr)
        {
            return;
        }

        updateTree(parent);

    }

    int getBalanceFactor(SumTreeNode* node = nullptr) const
    {
        if (node == nullptr) node = root;
        return node->getLeftHeight() - node->getRightHeight();
    }

    template <class A>
    void inorderAux(A& action, SumTreeNode* curr) const
    {
        if (curr == nullptr) return;
        inorderAux(action, curr->getLeft());
        action(
            curr->getLevel(),
            curr->getInThisLevel(),
            curr->getHeight(),
            curr->getLeft() == nullptr ? int() : curr->getLeft()->getLevel(),
            curr->getRight() == nullptr ? int() : curr->getRight()->getLevel(),
            curr->getParent() == nullptr ? int() : curr->getParent()->getLevel(),
            getBalanceFactor(curr)
        );
        inorderAux(action, curr->getRight());
    }

    /*
     * addNode auxiliary method adding node given findLocation's return values.
     */
    static void addNodeToLocation(SumTreeNode* location, SumTreeNode* newNode, Order orderRel)
    {
        if (orderRel == equal)
        {
            //Trying to add a node that was already in the tree.
            throw Failure("Trying to add a node that was already added.");
        }

        if (orderRel == larger)
        {
            assert(location->getRight() == nullptr);
            location->setRight(newNode);
        }
        else
        {
            assert(location->getLeft() == nullptr);
            location->setLeft(newNode);
        }
    }

    SumTreeNode* LLRotation(SumTreeNode* subtreeRoot)
    {
        SumTreeNode *oldRoot = subtreeRoot,
            *rootParent = subtreeRoot->getParent(),
            *newRoot = subtreeRoot->getLeft();

        //Set new root:
        if (rootParent != nullptr)
        {
            if (rootParent->getLevel() > oldRoot->getLevel())
            {
                rootParent->setLeft(newRoot);
            }
            else
            {
                rootParent->setRight(newRoot);
            }
        }
        else
        {
            root = newRoot;
            newRoot->setParent(nullptr);
        }

        //Pluck new root's old right child, and make it old root's new left:
        oldRoot->setLeft(newRoot->getRight());

        //Make old root the right child of the new root:
        newRoot->setRight(oldRoot);

        //Update the heights, totalLevels and Ws that were affected:
        oldRoot->updateHeight();
        newRoot->updateHeight();

        return newRoot;
    }

    SumTreeNode* RRRotation(SumTreeNode* subtreeRoot)
    {
        SumTreeNode *oldRoot = subtreeRoot,
                *rootParent = subtreeRoot->getParent(),
                *newRoot = subtreeRoot->getRight();

        //Set new root:
        if (rootParent != nullptr)
        {
            if (rootParent->getLevel() > oldRoot->getLevel())
            {
                rootParent->setLeft(newRoot);
            }
            else
            {
                rootParent->setRight(newRoot);
            }
        }
        else
        {
            root = newRoot;
            newRoot->setParent(nullptr);
        }

        //Pluck new root's old left child, and make it old root's new right:
        oldRoot->setRight(newRoot->getLeft());

        //Make old root the left child of the new root:
        newRoot->setLeft(oldRoot);

        //Update the heights, totalLevels and Ws that were affected:
        oldRoot->updateHeight();
        newRoot->updateHeight();

        return newRoot;
    }

    SumTreeNode* LRRotation(SumTreeNode* subtreeRoot)
    {
        SumTreeNode *rootParent = subtreeRoot->getParent(),
                *newRight = subtreeRoot,
                *newLeft = newRight->getLeft(),
                *newRoot = newLeft->getRight();

        //Set new root:
        if (rootParent != nullptr)
        {
            if (rootParent->getLevel() > subtreeRoot->getLevel())
            {
                rootParent->setLeft(newRoot);
            }
            else
            {
                rootParent->setRight(newRoot);
            }
        }
        else
        {
            root = newRoot;
            newRoot->setParent(nullptr);
        }

        //Rotation fun stuff:
        newRight->setLeft(newRoot->getRight());
        newLeft->setRight(newRoot->getLeft());
        newRoot->setLeft(newLeft);
        newRoot->setRight(newRight);

        //Update the heights that were affected:
        newRight->updateHeight();
        newLeft->updateHeight();
        newRoot->updateHeight();
        if (rootParent != nullptr) rootParent->updateHeight();

        return newRoot;
    }

    SumTreeNode* RLRotation(SumTreeNode* subtreeRoot)
    {
        SumTreeNode *rootParent = subtreeRoot->getParent(),
                *newLeft = subtreeRoot,
                *newRight = newLeft->getRight(),
                *newRoot = newRight->getLeft();

        //Set new root:
        if (rootParent != nullptr)
        {
            if (rootParent->getLevel() > subtreeRoot->getLevel())
            {
                rootParent->setLeft(newRoot);
            }
            else
            {
                rootParent->setRight(newRoot);
            }
        }
        else
        {
            root = newRoot;
            newRoot->setParent(nullptr);
        }

        //Rotation fun stuff:
        newRight->setLeft(newRoot->getRight());
        newLeft->setRight(newRoot->getLeft());
        newRoot->setLeft(newLeft);
        newRoot->setRight(newRight);

        //Update the heights that were affected:
        newRight->updateHeight();
        newLeft->updateHeight();
        newRoot->updateHeight();
        if (rootParent != nullptr) rootParent->updateHeight();

        return newRoot;
    }

    /*
     * Performs rotation based on subtreeRoot's balance factor and that of its children.
     * @param recursive determines if this should happen all the way up to the root.
     *        Should be set to true if rotate is called for node removal.
     * Returns the new root of the subtree post-rotation.
     */
    SumTreeNode* rotate(SumTreeNode* subtreeRoot, bool recursive=false)
    {
        SumTreeNode* newRoot = subtreeRoot;
        int rootBF = getBalanceFactor(subtreeRoot);
        if (abs(rootBF) > AVL_BALANCE_BOUND)
        {
            //rootBF is either 2==AVL_BALANCE_BOUND+1 (for Lx rotations) or minus that (for Rx rotations).
            assert(rootBF == (AVL_BALANCE_BOUND + 1) || rootBF == -(AVL_BALANCE_BOUND + 1));
            if (rootBF == AVL_BALANCE_BOUND + 1)
            {
                //Lx rotations
                assert(abs(getBalanceFactor(subtreeRoot->getLeft())) <= 1);

                if (getBalanceFactor(subtreeRoot->getLeft()) >= 0)
                {
                    newRoot = LLRotation(subtreeRoot);
                }
                else
                {
                    newRoot = LRRotation(subtreeRoot);
                }
            }
            else
            {
                //Rx rotations
                assert(abs(getBalanceFactor(subtreeRoot->getRight())) <= 1);
                if (getBalanceFactor(subtreeRoot->getRight()) <= 0)
                {
                    newRoot = RRRotation(subtreeRoot);
                }
                else
                {
                    newRoot = RLRotation(subtreeRoot);
                }
            }

            //This is for the case of node removal.
            if (recursive && newRoot->getParent() != nullptr)
            {
                return rotate(newRoot->getParent());
            }
        }
        return newRoot;
    }

    /*
     * Finds the next in order (inorder) in the specific relevant case within removeNode.
     */
    static SumTreeNode* removeNode_nextInorderAux(SumTreeNode* node)
    {
        assert(node != nullptr && node->getRight() != nullptr);
        node = node->getRight();
        while (node->getLeft() != nullptr)
        {
            node = node->getLeft();
        }
        return node;
    }

    /*
     * Finds the previous in order (inorder) of highest in the specific relevant case within removeNode.
     */
    void removeNode_updateHighestAux(SumTreeNode* node)
    {
        assert(node != nullptr && highest != nullptr);
        if (node->getLevel() != highest->getLevel())
        {
            return;
        }
        if (node->getLeft() != nullptr)
        {
            highest = node->getLeft();
            while (highest->getRight() != nullptr)
            {
                highest = highest->getRight();
            }
        }
        else if (node->getParent() != nullptr)
        {
            highest = node->getParent();
        }
        else
        {
            highest = nullptr;
        }
    }

    /*
     * Aux of removeNode to remove a leaf. Returns its parent, or nullptr if it was the root.
     */
    SumTreeNode* removeNode_LeafAux(SumTreeNode* node)
    {
        assert (node != nullptr && node->getHeight() == 0);
        SumTreeNode* parent = node->getParent();
        if (parent != nullptr)
        {
            //Can't just check if value > parent value because sorting may momentarily be broken
            //after using swapNodes for node removal.
            //if (parent->getLeft() != nullptr && node->getLevel() == parent->getLeft()->getLevel())
            if (parent->getLeft() == node)
            {
                parent->setLeft(nullptr);
            }
            else
            {
                parent->setRight(nullptr);
            }
        }
        else
        {
            root = nullptr;
        }
        delete node;
        return parent;
    }

    /*
     * Aux of removeNode to remove a node with only one child. Returns its parent or nullptr if root.
     */
    SumTreeNode* removeNode_OneChildAux(SumTreeNode* node)
    {
        assert (node->getLeft() == nullptr || node->getRight() == nullptr);
        SumTreeNode *parent = node->getParent(), *child = node->getLeft();
        if (child == nullptr) child = node->getRight();

        if (parent == nullptr)
        {
            root = child;
            child->setParent(nullptr);
        }
        else
        {
            if (parent->getLeft() == node)
            {
                parent->setLeft(child);
            }
            else
            {
                parent->setRight(child);
            }
        }
        delete node;
        return parent;
    }

    /*
     * Removes the node as though it's a normal binary search tree.
     * Returns the lowest node affected.
     */
    SumTreeNode* removeNodeAux(SumTreeNode* node)
    {
        assert (node != nullptr);

        //Leaf:
        if (node->getHeight() == 0)
        {
            return removeNode_LeafAux(node);
        }
            //Only one descendant:
        else if (node->getLeft() == nullptr || node->getRight() == nullptr)
        {
            return removeNode_OneChildAux(node);
        }
        else
        {
            //Swap node with its successor in the inorder sense:
            SumTreeNode* nextInOrder = removeNode_nextInorderAux(node);
            swapNodes(node, nextInOrder);

            //We're now looking at a node which either only has one descendant (since no left child),
            //or is a leaf.
            //Call the function again to enter one of the previous blocks.
            return removeNodeAux(nextInOrder);
        }
    }

    void swapNodes(SumTreeNode* a, SumTreeNode* b)
    {
        assert(a != nullptr && b != nullptr);

        //Swapping highest ptr too if needed:
        if (highest->getLevel() == a->getLevel())
        {
            highest = b;
        }
        else if (highest->getLevel() == b->getLevel())
        {
            highest = a;
        }

        //If highest is root then it can only have at most 1 left child, so we won't get here.
        //if ()

        a->swap(b);
    }

public:
    explicit SumTree(): levelZero(0), root(nullptr), highest(nullptr), nodeCount(0)
    {}

    void removeNode(int level)
    {
        if (level == 0)
        {
            if (levelZero == 0)
            {
                throw Failure("Tried to remove non-existent node (levelZero, removeNode).");
            }
            --levelZero;
            return;
        }
        Order orderRel;
        SumTreeNode* node = findLocation(level, orderRel);
        if (node == nullptr || orderRel != equal)
        {
            //Node isn't in the tree.
            throw Failure("Tried to remove non-existent node.");
        }
        node->decreaseInThisLevel();
        SumTreeNode* nodeToFix;
        if (node->getInThisLevel() == 0)
        {
            removeNode_updateHighestAux(node);
            nodeToFix = removeNodeAux(node);
            --nodeCount;
        }
        else
        {
            nodeToFix = node->getParent();
        }

        updateTree(nodeToFix);
    }

    int getLevelZero() const
    {
        return levelZero;
    }

    void addNode(int level, int inThisLevel = 1)
    {
        if (level == 0)
        {
            ++levelZero;
            return;
        }

        SumTreeNode* newNode(new SumTreeNode(level, inThisLevel));

        if (root == nullptr)
        {
            root = highest = newNode;
            ++nodeCount;
        }
        else
        {
            Order orderRel;
            SumTreeNode* location = findLocation(level, orderRel);
            if (orderRel == equal)
            {
                location->increaseInThisLevel();
                delete newNode;
                newNode = location;
            }
            else
            {
                addNodeToLocation(location, newNode, orderRel);
                ++nodeCount;
            }

            updateTree(newNode);

            assert(highest != nullptr);
            if (highest->getLevel() < newNode->getLevel())
            {
                highest = newNode;
            }
        }
    }

    /*int getHighest() const
    {
        return highest->getLevel();
    }*/

    template <class A>
    void inorder(A& action) const
    {
        inorderAux(action, root);
    }

    //WITHOUT LEVELZERO.
    int getSize() const
    {
        return this->nodeCount;
    }

    int getPlayerCount() const
    {
        return levelZero + (root == nullptr ? 0 : root->getW());
    }

    int getRootWDebug() const
    {
        return this->root->getW();
    }

    /*
     * This function emulates searching an object by key. (For example: getValuePtr(Group(3)) to find group with id==3.)
     */
    /*template <class K>
    T* getLevelPtr(K key)
    {
        bool found;
        SumTreeNode* node = findByKey(key, &found);
        if (!found)
        {
            throw Failure("Tried to fetch non-existent node.");
        }
        return &(node->getLevel());
    }*/

    /*template <class K>
    bool isInTree(K key)
    {
        bool found;
        findByKey(key, &found);
        return found;
    }*/

    static std::unique_ptr<SumTree> treeFromArray(int* arr, int* levels, int size)
    {
        return StaticAVLUtilities::AVLFromArray(arr, levels, size);
    }

    static int* treeToArray(const SumTree& tree, int** levels, bool reverse=false)
    {
        return StaticAVLUtilities::treeToArray(tree, levels, reverse);
    }

    /*
     * THIS RUINS THE PARAMETER TREES! CAREFUL!
     */
    static SumTree* mergeTrees(SumTree& t1, SumTree& t2)
    {
        return StaticAVLUtilities::mergeTrees(t1, t2).release();
    }

    int countInRange(int lowerRange, int higherRange) const
    {
        SumTreeNode *bottom, *top;
        Order orderRel;
        findLocationBounds(higherRange, orderRel, &top, nullptr);
        int inTopLevel = top == nullptr ? 0 : top->getInThisLevel();
        if (lowerRange > 0)
        {
            findLocationBounds(lowerRange, orderRel, nullptr, &bottom);
            return lowerThan(top, false) + inTopLevel - (bottom == nullptr ? 0 : lowerThan(bottom, true));
        }
        return lowerThan(top, false) + inTopLevel + getLevelZero();
    }

    //This should only be called if m <= player count.
    int sumLevelOfTopM(int m) const
    {
        if (m > getPlayerCount())
        {
            throw Failure("sumLevelOfTopM: illegal m.");
        }
        if (root == nullptr)
        {
            return 0; //They're all level 0.
        }

        int leftToSum = m, sum = 0;
        SumTreeNode* curr = root;

        if (leftToSum > root->getW())
        {
            leftToSum = root->getW(); //Take minimum amount from levelZeros.
        }

        while (leftToSum > 0)
        {
            if (curr->getRightW() >= leftToSum)
            {
                curr = curr->getRight();
            }
            else
            {
                sum += curr->getRightTotalLevel();
                leftToSum -= curr->getRightW();
                if (leftToSum <= curr->getInThisLevel())
                {
                    return sum + leftToSum * curr->getLevel();
                }
                else
                {
                    sum += curr->getInThisLevel() * curr->getLevel();
                    leftToSum -= curr->getInThisLevel();
                    curr = curr->getLeft();
                }
            }
        }

        throw Failure("Got to a weird place in sumTopM thingy in SumTree.");
        return -1;
    }

    void clean()
    {
        this->freeList();
        this->root = nullptr;
        this->highest = nullptr;
        this->nodeCount = 0;
    }

    SumTree(SumTree& other) = delete;
    SumTree& operator=(SumTree& other) = delete;
    ~SumTree()
    {
        freeList();
    }
};

#endif //AVLTree_HPP 