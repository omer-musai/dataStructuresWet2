#ifndef SUMTREE_BIDIRECTIONAL_NODE
#define SUMTREE_BIDIRECTIONAL_NODE

#include <cassert>

//A bidirectional node to be used for a our Sum Tree.
//We won't make it generic this time.
class SumTreeNode
{
private:
    SumTreeNode* left;
    SumTreeNode* right;
    SumTreeNode* parent;
	
	int level; //This will be the key.
	int inThisLevel;
	int totalLevel;
	int w;
	int height;

public:
    explicit SumTreeNode(int level, int inThisLevel = 1)
        :   left(nullptr), right(nullptr), parent(nullptr), level(level),
            inThisLevel(inThisLevel), totalLevel(level), w(1), height(0)
    {
        updateHeight();
    }

    SumTreeNode& operator=(SumTreeNode& other) = delete;

    SumTreeNode() = delete;
    ~SumTreeNode() = default;

    void setLeft(SumTreeNode* newLeft)
    {
        this->left = newLeft;
        if (newLeft != nullptr)
        {
            newLeft->parent = this;
        }
    }

    void increaseInThisLevel()
    {
        ++(this->inThisLevel);
        this->totalLevel += level;
        ++(this->w);
    }

    void decreaseInThisLevel()
    {
        --(this->inThisLevel);
        this->totalLevel -= level;
        --(this->w);
    }

    void setRight(SumTreeNode* newRight)
    {
        this->right = newRight;
        if (newRight != nullptr)
        {
            newRight->parent = this;
        }
    }

    void setParent(SumTreeNode* newParent)
    {
        this->parent = newParent;
    }

    SumTreeNode* getLeft()
    {
        return left;
    }

    SumTreeNode* getRight()
    {
        return right;
    }

    SumTreeNode* getParent()
    {
        return parent;
    }

    void updateHeight()
    {
        int lh = this->getLeftHeight(), rh = this->getRightHeight();
        this->height = 1 + (lh > rh ? lh : rh);

        this->w = this->getLeftW() + this->getRightW() + this->inThisLevel;
        this->totalLevel = this->getLeftTotalLevel() + this->getRightTotalLevel() + inThisLevel * level;
    }

    int getHeight() const
    {
        return height;
    }

    int getLevel() const
    {
        return level;
    }
	
	int getInThisLevel() const
    {
        return inThisLevel;
    }
	
	int getW() const
    {
        return w;
    }

    int getLeftHeight() const
    {
        return left == nullptr ? -1 : left->height;
    }

    int getLeftW() const
    {
        return left == nullptr ? 0 : left->w;
    }

    int getRightW() const
    {
        return right == nullptr ? 0 : right->w;
    }

    int getLeftTotalLevel() const
    {
        return left == nullptr ? 0 : left->totalLevel;
    }

    int getRightTotalLevel() const
    {
        return right == nullptr ? 0 : right->totalLevel;
    }

    int getRightHeight() const
    {
        return right == nullptr ? -1 : right->height;
    }

    void swap(SumTreeNode* other)
    {
        int temp = other->level;
        other->level = level;
        level = temp;

        temp = other->inThisLevel;
        other->inThisLevel = inThisLevel;
        inThisLevel = temp;

        //The rest of the fields get fixed by updateHeight calls.
    }
};

#endif //AVLTREE_BIDIRECTIONAL_NODE
