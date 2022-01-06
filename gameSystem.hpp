#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H




class GameSystem
{
    private:
        Group players_by_level;
        HashTable players;
        UF groups;

    public:
        GameSystem(int k, int scale) : players_by_level(scale), players(k), groups(k, scale) {}
        StatusType mergeGroups(int Id1, int id2);
        StatusType addPlayer(int playerId, int groupId, int score);
        StatusType removePlayer(int playerId);
        StatusType increasePlayerIDLevel(int playerId, int levelincrease);
        StatusType changePlayerIDScore(int playerId, int newScore);
        StatusType getPercentOfPlayersWithScoreInBounds(int groupId, int score, int lowerLevel, int higherLevel, double* players);
        StatusType averageHighestPlayerLevelByGroup(int groupId, int m, double* avgLevel);
        StatusType getPlayersBound(int groupId, int score, int m, int* lowerBoundPlayers, int* higherBoundPlayers);



}
#endif //GAME_SYSTEM_H