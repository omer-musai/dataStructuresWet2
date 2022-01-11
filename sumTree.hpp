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
            int** array;
            int size;
            int index;
            bool reverse;

        public:
            ArrayFromTreePopulator() = delete;
            explicit ArrayFromTreePopulator(int** array, int size = -1, bool reverse=false):
                    array(array), size(size), index(0), reverse(reverse)
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

            void operator()(int& value, int height, int parentValue, int leftValue, int rightValue, int BF) {
                if (reverse)
                {
                    if(size < 0 || index >= 0)
                    {
                        array[index--] = &value;
                    }
                }
                else
                {
                    if (size < 0 || index < size)
                    {
                        array[index++] = &value;
                    }
                }
            }
        };

        static int** treeToArray(const SumTree& tree, bool reverse=false) {
            int** array = new int*[tree.getSize()];
            try
            {
                ArrayFromTreePopulator populator(array, tree.getSize(), reverse);
                tree.inorder(populator);
            }
            catch (std::exception& exception)
            {
                delete[] array;
                throw exception;
            }
            return array;
        }

        static int** arrayMerge(int** arr1, int size1, int** arr2, int size2) {
            int** array = new int*[size1 + size2];
            try
            {
                int i1 = 0, i2 = 0, i = 0;
                while (i1 < size1 && i2 < size2) {
                    array[i] = (*arr1[i1]) < (*arr2[i2]) ? arr1[i1] : arr2[i2];
                    if (array[i] == arr1[i1]) {
                        ++i1;
                    } else {
                        ++i2;
                    }
                    ++i;
                }
                while (i1 < size1) {
                    array[i++] = arr1[i1++];
                }
                while (i2 < size2) {
                    array[i++] = arr2[i2++];
                }
            }
            catch (std::out_of_range& exception)
            {
                delete[] array;
            }

            return array;
        };

        //This uses the algorithm described & proved in the doc.
        static std::shared_ptr<SumTree> AVLFromArray(int** arr, int size, bool clone=true) {
            assert(size >= 0);

            std::shared_ptr<SumTree> tree = std::shared_ptr<SumTree>(new SumTree());
            if (size > 0)
            {
                int m = (size % 2 == 0 ? size / 2 : (size + 1) / 2) - 1; //m=ceil(size/2)-1
                tree->addNode(*arr[m]); //O(1) since the tree is empty.

                if (size == 2) {
                    tree->addNode(*arr[m + 1]); //O(1) since the tree is always sized 1 and this point.
                } else if (size > 2) {
                    tree->setLeftSubtree(AVLFromArray(arr, m, clone));
                    tree->setRightSubtree(AVLFromArray(arr + (m + 1), size - m - 1, clone));
                }
            }

            return tree;
        }

        //THIS RUINS THE PARAMETER TREES. Careful!
        static std::shared_ptr<SumTree> mergeTrees(SumTree& t1, SumTree& t2, bool clone=true) {
            int **t1arr = nullptr, **t2arr = nullptr, **merged = nullptr;
            int totalSize = t1.getSize() + t2.getSize();
            std::shared_ptr<SumTree> result;
            try {
                t1arr = treeToArray(t1);
                t2arr = treeToArray(t2);
                merged = arrayMerge(t1arr, t1.getSize(), t2arr, t2.getSize());
                result = treeFromArray(merged, totalSize, clone);
                t1.clean();
                t2.clean();
            }
            catch (std::exception &exception) {

            }

            if (t1arr != nullptr)
            {
                delete[] t1arr;
            }
            if (t2arr != nullptr)
            {
                delete[] t2arr;
            }
            if (merged != nullptr)
            {
                delete[] merged;
            }

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

        //Update the height that was affected:
        oldRoot->updateHeight();

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

        //Update the height that was affected:
        oldRoot->updateHeight();

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
        removeNode_updateHighestAux(node);
        SumTreeNode* nodeToFix = removeNodeAux(node);
        --nodeCount;

        updateTree(nodeToFix);
    }

    int getLevelZero() const
    {
        return levelZero;
    }

    void addNode(int level)
    {
        if (level == 0)
        {
            ++levelZero;
            return;
        }

        SumTreeNode* newNode(new SumTreeNode(level));

        if (root == nullptr)
        {
            root = highest = newNode;
        }
        else
        {
            Order orderRel;
            SumTreeNode* location = findLocation(level, orderRel);
            if (orderRel == equal)
            {
                location->increaseInThisLevel();
                delete newNode;
            }
            else
            {
                addNodeToLocation(location, newNode, orderRel);
            }

            updateTree(newNode);

            assert(highest != nullptr);
            if (highest->getLevel() < newNode->getLevel())
            {
                highest = newNode;
            }
        }

        ++nodeCount;
    }

    const int& getHighest() const
    {
        return highest->getLevel();
    }

    template <class A>
    void inorder(A& action) const
    {
        inorderAux(action, root);
    }

    int getSize() const
    {
        return this->nodeCount;
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

    static std::shared_ptr<SumTree> treeFromArray(int** arr, int size, bool clone=true)
    {
        return StaticAVLUtilities::AVLFromArray(arr, size, clone);
    }

    static int** treeToArray(const SumTree& tree, bool reverse=false)
    {
        return StaticAVLUtilities::treeToArray(tree, reverse);
    }

    /*
     * THIS RUINS THE PARAMETER TREES! CAREFUL!
     */
    static std::shared_ptr<SumTree> mergeTrees(SumTree& t1, SumTree& t2)
    {
        return StaticAVLUtilities::mergeTrees(t1, t2);
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