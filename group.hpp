#ifndef GROUP_H
#define GROUP_H

#include "sumTree.hpp"
#include <memory>

class Group
{
    private:
        int scale;
        std::shared_ptr<std::shared_ptr<SumTree>> trees_array;

    public:
        
        Group(int scale) : scale(scale), trees_array(new std::shared_ptr<SumTree>[scale + 1]) {}
       
        Group(Group& g) = delete;
        Group& operator=(Group& g) = delete;

        void merge_groups(Group& g)
        {
            for (int i = 0; i < scale + 1; i++)
            {
                this->trees_array[i] = merge_trees(this->trees_array[i], g.trees_array[i]);
            }
        }
};

#endif //GROUP_H