#include "GroupsUnionFind.hpp"

/*
 * Receives the ID of a group, and returns the ID of its superset: the number of the group at the root
 * of the disjoint set in which the group with this ID is contained.
 */
int GroupsUnionFind::findGroupId(int id)
{
    if (id <= 0 || id > k)
    {
        throw InvalidInput("Invalid group ID passed to findGroup.");
    }

    //Using id-1 because the array has an offset.
    int root, curr = id - 1, next;

    root = findRoot(id);

    //Path compression:
    if (root != curr)
    {
        while (parents[curr] != root) {
            next = parents[curr] - 1;
            parents[curr] = root;
            curr = next;
        }
    }

    return root;
}

int GroupsUnionFind::findRoot(int groupId)
{
    int root = groupId - 1;
    while(parents[root] != 0)
    {
        root = parents[root - 1];
    }
    return root;
}

Group& GroupsUnionFind::findGroup(int id)
{
    return sets[findGroupId(id)];
}


Group& GroupsUnionFind::uniteGroups(int id1, int id2)
{
    id1 = findGroupId(id1); //Getting the actual group ID (i.e the root of the union).
    id2 = findGroupId(id2);
    int from = id1 <= id2 ? id1 : id2,
            to = id1 <= id2 ? id2 : id1;

    parents[from] = to;

    sets[to].mergeGroups(sets[from]);

    return sets[to];
}

GroupsUnionFind::GroupsUnionFind(int k, int scale) : sets(new Group[k]), sizes(new int[k]), parents(new int[k]), k(k), scale(scale)
{
    for (int i=0; i < k; i++)
    {
        sets[i].init(scale);
        sizes[i] = 0;
        parents[i] = 0;
    }
}

GroupsUnionFind::~GroupsUnionFind()
{
    delete[] sets;
    delete[] sizes;
    delete[] parents;
}
