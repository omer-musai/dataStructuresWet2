#include "library2.h"
#include "GameSystem.hpp"
#include "game_exceptions.hpp"

#define TRY_CATCH_WRAP(action) \
if (DS == NULL)                \
{                              \
    return INVALID_INPUT;      \
}                              \
try {                          \
    action                     \
}                              \
catch(Failure& exc)            \
{                              \
    return FAILURE;            \
}                              \
catch(std::bad_alloc& exc)     \
{                              \
    return ALLOCATION_ERROR;   \
}                              \
catch(InvalidInput& exc)       \
{                              \
    return INVALID_INPUT;      \
}                              \
return SUCCESS

//TODO:
//* Ensure order of exceptions.
//* Ensure every time FAILURE is thrown, the data structure returns to its previous state.

void *Init(int k, int scale)
{
    try
    {
        GameSystem* DS = new GameSystem(k, scale);
        return (void*)DS;
    }
    catch (std::bad_alloc& exc)
    {
        return NULL;
    }
}

StatusType MergeGroups(void *DS, int GroupID1, int GroupID2)
{
    TRY_CATCH_WRAP(
    ((GameSystem*)DS)->mergeGroups(GroupID1, GroupID2);
    );
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int score)
{
    TRY_CATCH_WRAP(
    ((GameSystem*)DS)->addPlayer(PlayerID, GroupID, score);
    );
}

StatusType RemovePlayer(void *DS, int PlayerID)
{
    TRY_CATCH_WRAP(
    ((GameSystem*)DS)->removePlayer(PlayerID);
    );
}

StatusType IncreasePlayerIDLevel(void *DS, int PlayerID, int LevelIncrease)
{
    TRY_CATCH_WRAP(
    ((GameSystem*)DS)->increasePlayerIDLevel(PlayerID, LevelIncrease);
    );
}

StatusType ChangePlayerIDScore(void *DS, int PlayerID, int NewScore)
{
    TRY_CATCH_WRAP(
    ((GameSystem*)DS)->changePlayerIDScore(PlayerID, NewScore);
    );
}

StatusType GetPercentOfPlayersWithScoreInBounds(void *DS, int GroupID, int score, int lowerLevel, int higherLevel,
                                            double * players)
{
    if (players == nullptr) return INVALID_INPUT;
    TRY_CATCH_WRAP(
    *players = ((GameSystem*)DS)->getPercentOfPlayersWithScoreInBounds(
            GroupID, score, lowerLevel, higherLevel
        );
    );
}

StatusType AverageHighestPlayerLevelByGroup(void *DS, int GroupID, int m, double * level)
{
    if (level == nullptr) return INVALID_INPUT;
    TRY_CATCH_WRAP(
    *level = ((GameSystem*)DS)->averageHighestPlayerLevelByGroup(GroupID, m);
    );
}

StatusType GetPlayersBound(void *DS, int GroupID, int score, int m,
                                         int * LowerBoundPlayers, int * HigherBoundPlayers)
{
    return FAILURE; //Not implemented.
}

void Quit(void** DS)
{
    delete ((GameSystem*)*DS);
    *DS = nullptr;
}