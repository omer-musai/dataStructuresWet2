#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "Group.hpp"
#include <memory>


class GroupsUnionFind
{
    private:
        Group* sets;
        int* sizes;
        int* parents;
        int k;
        int scale;

        int findGroupId(int id);

        int findRoot(int groupId);

    public:
        GroupsUnionFind(int k, int scale);

        GroupsUnionFind& operator=(const GroupsUnionFind& other) = delete;
        GroupsUnionFind(const GroupsUnionFind& other) = delete;

        Group& findGroup(int id);

        Group& uniteGroups(int id1, int id2);

        ~GroupsUnionFind();
};
#endif //UNION_FIND_H