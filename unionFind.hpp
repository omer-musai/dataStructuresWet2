#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "group.hpp"
#include <memory>


class UF
{
    private:
        std::shared_ptr<Group> sets;
        std::shared_ptr<int> sizes;
        std::shared_ptr<int> parents;

    public:
        UF(int k, int scale)
        {
            sets = new Group[k];
            sizes = new int[k];
            parents = new int[k];
            
            for (int i=0; i < k; i++)
            {
                sizes[i] = 1;
                parents[i] = 0;
            }
           
        }

       
        Group find(int id)
        {
            int root = id-1;
            int curr = id-1;
            int tmp =0;
           
            //find root
            while(this->parents[root] != 0)
            {
                root = this->parents[root-1];
            }

            //change elements to point to the root
            while(this->parents[curr] != root)
            {
                tmp = this->parents[curr] -1;
                this->parents[tmp] = root;
                curr = tmp;
            }

            return sets[root];
        }
        
        
        Group union(int id1, int id2)
        {

        }
};
#endif //UNION_FIND_H