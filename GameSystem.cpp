#include "GameSystem.hpp"

void GameSystem::mergeGroups(int id1, int id2)
{
    if (id1 <= 0 || id2 <= 0)
    {
        throw InvalidInput("Invalid input to mergeGroups.");
    }

    groups.uniteGroups(id1, id2);
}

void GameSystem::addPlayer(int playerId, int groupId, int score)
{
    Player p(playerId, groupId, score);
    addPlayer(p);
}

void GameSystem::addPlayer(const Player& player)
{
    if (player.getPlayerId() <= 0 || player.getScore() <= 0 || player.getScore() > scale)
    {
        throw InvalidInput("Invalid input to addPlayer.");
    }

    Group& group = groups.findGroup(player.getGroupId()); //This also ensures the group exists.

    players.insert(player);
    group.addPlayer(player);
    players_by_level.addPlayer(player);
}

void GameSystem::removePlayer(int playerId)
{
    if (playerId <= 0)
    {
        throw InvalidInput("Invalid input to removePlayer.");
    }

    const Player& p = players.search(playerId);
    groups.findGroup(p.getGroupId()).removePlayer(p);
    players_by_level.removePlayer(p);
    players.remove(p.getPlayerId());
}

void GameSystem::increasePlayerIDLevel(int playerId, int levelIncrease)
{
    if (playerId <= 0 || levelIncrease <= 0)
    {
        throw InvalidInput("Invalid input to increasePlayerIDLevel.");
    }

    Player player = players.search(playerId);
    player.setLevel(player.getLevel() + levelIncrease);
    removePlayer(playerId);
    addPlayer(player);
}

void GameSystem::changePlayerIDScore(int playerId, int newScore)
{
    if (playerId <= 0 || newScore <= 0 || newScore > scale)
    {
        throw InvalidInput("Invalid input to changePlayerIDScore.");
    }

    Player p = players.search(playerId);
    removePlayer(playerId);
    addPlayer(p.getPlayerId(), p.getGroupId(), newScore);
}

double GameSystem::getPercentOfPlayersWithScoreInBounds(int groupId, int score, int lowerLevel, int higherLevel)
{
    if (groupId < 0 || groupId > k) //TODO: ensure the library2.cpp version of this checks for nullptr.
    {
        throw InvalidInput("Invalid input to getPercentOfPlayersWithScoreInBounds.");
    }

    if (lowerLevel > higherLevel || score > scale)
    {
        return 0;
    }

    const Group& group = groupId > 0 ? groups.findGroup(groupId) : players_by_level;

    double playersInRange = group.countPlayersInRange(lowerLevel, higherLevel);
    if (playersInRange == 0)
    {
        return 0;
    }

    return ((double)group.countPlayersWithScoreInRange(lowerLevel, higherLevel, score)
        / playersInRange) * 100;
}

double GameSystem::averageHighestPlayerLevelByGroup(int groupId, int m)
{
    if (groupId < 0 || groupId > k || m <= 0)
    {
        throw InvalidInput("Invalid input to averageHighestPlayerLevelByGroup.");
    }

    Group& group = groupId > 0 ? groups.findGroup(groupId) : players_by_level;
    if (m > group.getPlayerCount())
    {
        throw Failure("m > player count in averageHighestPlayerLevelByGroup.");
    }

    //TODO: implement, AND THROW FAILURE if 0 characters in range.
    return (double)group.sumLevelOfTopM(m) / m;
}

void GameSystem::getPlayersBound(int groupId, int score, int m, int *lowerBoundPlayers, int *higherBoundPlayers) const
{
    if (groupId < 0 || groupId > k || score <= 0 || score > scale || m < 0
        || lowerBoundPlayers == nullptr || higherBoundPlayers == nullptr) //TODO: ensure m < 0 is right here. In the prev it was <=
    {
        throw InvalidInput("Invalid input to getPercentOfPlayersWithScoreInBounds.");
    }
}