#ifndef GROUP_H
#define GROUP_H

#include "SumTree.hpp"
#include "Player.hpp"
#include <memory>

class Group
{
    private:
        int scale;
        SumTree** trees_array;
        int playerCount;
        bool initialized;
        int countPlayersInRange_Aux(int lowerLevel, int higherLevel, int score=-1) const;

    public:
        Group() : scale(-1), trees_array(nullptr), playerCount(0), initialized(false) {} //For array initialization.
        explicit Group(int scale);

        void init(int scale);

        Group(Group& g) = delete;
        Group& operator=(Group& g) = delete;

        bool assertDebug() const
        {
            assert(trees_array[0]->getPlayerCount() == playerCount);
            return trees_array[0]->getPlayerCount() == playerCount;
        }

        void addPlayer(const Player& player);

        void removePlayer(const Player& player);

        SumTree** getPlayers() const;

        void mergeGroups(Group& g);

        int countPlayersWithScoreInRange(int lowerLevel, int higherLevel, int score) const;

        int countPlayersInRange(int lowerLevel, int higherLevel) const;

        int getPlayerCount() const;

        int sumLevelOfTopM(int m) const;

        void clean();

        ~Group();
};

#endif //GROUP_H