#include "Group.hpp"

int Group::countPlayersInRange_Aux(int lowerLevel, int higherLevel, int score) const
{
    SumTree* tree = score < 0 ? trees_array[0] : trees_array[score];

    return tree->countInRange(lowerLevel, higherLevel);
}

Group::Group(int scale) : scale(scale), trees_array(nullptr), playerCount(0), initialized(false)
{
    init(scale);
}

void Group::init(int scale)
{
    if (initialized)
    {
        throw Failure("Tried to initialize Group object twice.");
    }
    this->scale = scale;
    int cnt = -1;
    try
    {
        trees_array = new SumTree*[scale + 1];
        for (cnt = 0; cnt < scale + 1; ++cnt)
        {
            trees_array[cnt] = new SumTree();
        }
    }
    catch (const std::exception& exc)
    {
        if (cnt >= 0)
        {
            for (int i = 0; i < cnt; ++i) delete trees_array[i];
            delete[] trees_array;
        }
        throw AllocationError("Group init failed.");
    }
    initialized = true;
}

void Group::addPlayer(const Player &player)
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (addPlayer).");
    }

    trees_array[0]->addNode(player.getLevel()); //All players tree.
    trees_array[player.getScore()]->addNode(player.getLevel()); //Score-based tree.
    ++playerCount;
}

void Group::removePlayer(const Player &player)
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (removePlayer).");
    }

    trees_array[0]->removeNode(player.getLevel()); //All players tree.
    trees_array[player.getScore()]->addNode(player.getLevel()); //Score-based tree.
    --playerCount;
}

SumTree** Group::getPlayers() const
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (getPlayers).");
    }
    return trees_array;
}

void Group::mergeGroups(Group &g)
{
    if (!initialized || !g.initialized)
    {
        throw Failure("Tried to use uninitialized group (mergeGroups).");
    }

    for (int i = 0; i < scale + 1; i++)
    {
        SumTree *toFree1 = trees_array[i], *toFree2 = g.trees_array[i];
        trees_array[i]
                = SumTree::mergeTrees(*trees_array[i], *g.trees_array[i]);
        delete toFree1;
        delete toFree2;
    }
}

int Group::countPlayersWithScoreInRange(int lowerLevel, int higherLevel, int score) const
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (countPlayersWithScoreInRange).");
    }

    return countPlayersInRange_Aux(lowerLevel, higherLevel, score);
}

int Group::countPlayersInRange(int lowerLevel, int higherLevel) const
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (countPlayersInRange).");
    }

    return countPlayersInRange_Aux(lowerLevel, higherLevel);
}

int Group::getPlayerCount() const
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (getPlayerCount).");
    }

    return playerCount;
}

int Group::sumLevelOfTopM(int m) const
{
    if (!initialized)
    {
        throw Failure("Tried to use uninitialized group (sumLevelOfTopM).");
    }

    return trees_array[0]->sumLevelOfTopM(m);
}

void Group::clean()
{
    if (!initialized) return;

    for (int i = 0; i < scale + 1; ++i)
    {
        delete trees_array[i];
    }
    delete[] trees_array;
}

Group::~Group()
{
    clean();
}
