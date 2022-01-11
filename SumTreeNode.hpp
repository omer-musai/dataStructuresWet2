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
    explicit SumTreeNode(int level=0) : level(level), inThisLevel(1), totalLevel(1), w(1), left(nullptr), right(nullptr), parent(nullptr), height(0)
    {}

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
        ++(this->totalLevel);
        ++(this->w);
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

    int& getLevel() //TODO: consdier making this const (check if this is bad, RE: SumTree->addNode).
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
	
	int getTotalLevel() const
    {
        return totalLevel;
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

        temp = other->w;
    }
};

#endif //AVLTREE_BIDIRECTIONAL_NODE
