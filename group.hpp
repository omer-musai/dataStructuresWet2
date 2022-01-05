#ifndef GROUP_H
#define GROUP_H

#include "sumTree.hpp"

class Group
{
    private:
        static int scale;
        SumTree* trees_array



    public:
        Group()
        {
            trees_array = new SumTree[scale + 1];
        }

        Group(int scale) : scale(scale)
        {
            trees_array = new SumTree[scale + 1];
        }




        

}

#endif //GROUP_H