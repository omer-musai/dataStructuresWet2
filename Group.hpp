#ifndef GROUP_H
#define GROUP_H

#include "SumTree.hpp"
#include "Player.hpp"
#include <memory>

class Group
{
    private:
        int scale;
        std::shared_ptr<std::shared_ptr<SumTree>> trees_array;

    public:
        Group() : scale(-1), trees_array(nullptr) {} //For array initialization.
        Group(int scale) : scale(scale), trees_array(new std::shared_ptr<SumTree>[scale + 1]) {}

        Group(Group& g) = delete;
        Group& operator=(Group& g) = delete;

        void addPlayer(const Player& player)
        {
            trees_array.get()[0]->addNode(player.getLevel()); //All players tree.
            trees_array.get()[player.getScore()]->addNode(player.getLevel()); //Score-based tree.
        }

        void removePlayer(const Player& player)
        {
            trees_array.get()[0]->removeNode(player.getLevel()); //All players tree.
            trees_array.get()[player.getScore()]->addNode(player.getLevel()); //Score-based tree.
        }

        std::shared_ptr<std::shared_ptr<SumTree>> getPlayers() const
        {
            return trees_array;
        }

        void mergeGroups(Group& g)
        {
            for (int i = 0; i < scale + 1; i++)
            {
                trees_array.get()[i]
                    = SumTree::mergeTrees(*trees_array.get()[i], *g.trees_array.get()[i]);
            }
        }
};

#endif //GROUP_H