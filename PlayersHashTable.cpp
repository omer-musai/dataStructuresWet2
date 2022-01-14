#include "PlayersHashTable.hpp"

int PlayersHashTable::hash(int playerId) const
{
    return playerId % tableLength;
}

//This moves this->table into newTable using the new hash function, frees this->table, and sets
//it to newTable.
void PlayersHashTable::replaceTable(Node** newTable, int oldLength)
{
    for (int cnt = 0; cnt < oldLength; ++cnt)
    {
        while (table[cnt] != nullptr)
        {
            insertNode(new Node(table[cnt]->getPlayer()), newTable);
            Node* toFree = table[cnt];
            table[cnt] = toFree->getNext();
            delete toFree;
        }
    }
    delete[] this->table;
    this->table = newTable;
}

void PlayersHashTable::expand()
{
    int oldLength = tableLength;
    tableLength *= expansionFactor;
    Node** newTable = new Node*[tableLength](); //The () inits to nullptrs.
    replaceTable(newTable, oldLength);
}

void PlayersHashTable::contract()
{
    assert(tableLength >= defaultStartingLength * expansionFactor);

    int oldLength = tableLength;
    tableLength /= expansionFactor;
    Node** newTable = new Node*[tableLength]();
    replaceTable(newTable, oldLength);
}

float PlayersHashTable::getLoadFactor() const
{
    return ((float)playerCount) / (float)tableLength;
}

void PlayersHashTable::rehash()
{
    float lf = getLoadFactor();
    if (lf >= maxLoadFactor)
    {
        expand();
    }
    else if (lf < minLoadFactor && tableLength > defaultStartingLength)
    {
        contract();
    }
}

void PlayersHashTable::insertNode(Node* node, Node** table)
{
    if (table == nullptr)
    {
        table = this->table;
    }

    int hashed = hash(node->getId());
    node->setNext(table[hashed]);
    table[hashed] = node;
}

void PlayersHashTable::insert(const Player& player)
{
    Node* node = findNode(player.getPlayerId());
    if (node != nullptr)
    {
        throw Failure("Tried to add a player that was already added.");
    }

    insertNode(new Node(player));
    ++playerCount;

    rehash(); //Expands if needed.
}

void PlayersHashTable::remove(int playerId)
{
    Node* node = findNode(playerId);
    if (node == nullptr)
    {
        throw Failure("Tried to remove non-existent player.");
    }

    //TODO: remove
    --playerCount;

    rehash(); //Contracts if needed.
}

PlayersHashTable::Node* PlayersHashTable::findNode(int playerId) const
{
    Node* current = table[hash(playerId)];
    while (current != nullptr)
    {
        if (current->getId() == playerId)
        {
            return current;
        }
        current = current->getNext();
    }
    return nullptr;
}

const Player& PlayersHashTable::search(int playerId) const
{
    Node* node = findNode(playerId);
    if (node == nullptr)
    {
        throw Failure("Player not found when searching hash table.");
    }

    return node->getPlayer();
}

bool PlayersHashTable::isMember(int playerId) const
{
    return findNode(playerId) != nullptr;
}

PlayersHashTable::~PlayersHashTable()
{
    for (int cnt = 0; cnt < tableLength; ++cnt)
    {
        while (table[cnt] != nullptr)
        {
            Node* temp = table[cnt];
            table[cnt] = temp->getNext();
            delete temp;
        }
    }

    delete[] table;
}