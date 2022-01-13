#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include "library2.h"
#include "Group.hpp"
#include "PlayersHashTable.hpp"
#include "GroupsUnionFind.hpp"

class GameSystem
{
    private:
        Group players_by_level;
        PlayersHashTable players;
        GroupsUnionFind groups;
        int k;
        int scale;
        void addPlayer(const Player& player);
    public:
        GameSystem(int k, int scale) : players_by_level(scale), players(), groups(k, scale), k(k), scale(scale) {}
        void mergeGroups(int id1, int id2);
        void addPlayer(int playerId, int groupId, int score);
        void removePlayer(int playerId);
        void increasePlayerIDLevel(int playerId, int levelIncrease);
        void changePlayerIDScore(int playerId, int newScore);
        double getPercentOfPlayersWithScoreInBounds(int groupId, int score, int lowerLevel, int higherLevel);
        double averageHighestPlayerLevelByGroup(int groupId, int m);
        void getPlayersBound(int groupId, int score, int m, int* lowerBoundPlayers, int* higherBoundPlayers) const;



};

#endif //GAME_SYSTEM_H