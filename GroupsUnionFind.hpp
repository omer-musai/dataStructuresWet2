#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "Group.hpp"
#include <memory>


class GroupsUnionFind
{
    private:
        std::shared_ptr<Group> sets;
        std::shared_ptr<int> sizes;
        std::shared_ptr<int> parents;
        int k;
        int scale;

        int findGroupId(int id);

        int findRoot(int groupId);

    public:
        GroupsUnionFind(int k, int scale);
       
        Group& findGroup(int id);

        Group& uniteGroups(int id1, int id2);
};
#endif //UNION_FIND_H