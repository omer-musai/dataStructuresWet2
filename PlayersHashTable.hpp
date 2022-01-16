#ifndef PLAYGROUND_PLAYERSHASHTABLE_HPP
#define PLAYGROUND_PLAYERSHASHTABLE_HPP

#include "Player.hpp"
#include "game_exceptions.hpp"
#include "GroupsUnionFind.hpp"
#include <cassert>

#include <iostream>//todo: remove

/*
 * Dynamic hash table using separate hashing and mod n (with n being the current table's tableLength)
 * as a hash function.
 */
class PlayersHashTable
{
private:
    class Node
    {
    private:
        const int illegal = -1;
        Player player;
        Node *next;

    public:
        Node(const Player& player, Node* next=nullptr)
                : player(player), next(next) //Copy it. Just a bunch of primitive fields.
        {}
        Node() : player(illegal, illegal, illegal), next(nullptr) //For array initialization.
        {}

        void setNext(Node *node)
        {
            next = node;
        }
        Node *getNext()
        {
            return next;
        }

        int getId() const
        {
            assert(this->player.getPlayerId() != illegal);
            return this->player.getPlayerId();
        }
        const Player& getPlayer() const
        {
            assert(this->player.getPlayerId() != illegal);
            return player;
        }
    };

    const int defaultStartingLength = 3;
    const int expansionFactor = 2;
    const float maxLoadFactor = 3.0/4.0;
    const float minLoadFactor = maxLoadFactor / 4;

    int tableLength;
    int playerCount;
    Node** table;

    int hash(int playerId) const;

    //This moves this->table into newTable using the new hash function, frees this->table, and sets
    //it to newTable.
    void replaceTable(Node** newTable, int oldLength);

    void expand();

    void contract();

    float getLoadFactor() const;

    //Expands or contracts if needed.
    void rehash();

    //Inserts node as the first link of the given table using PlayersHashTable::hash.
    //Auxiliary function for insert and replaceTable.
    void insertNode(Node* node, Node** table=nullptr);

    void removeNode(Node* node);

    Node* findNode(int playerId) const;
public:
    int countDebug(GroupsUnionFind* uf = nullptr) const
    {
        int bucket = 0;
        for (int cnt = 0; cnt < tableLength; ++cnt)
        {
            Node *curr = table[cnt];
            while (curr != nullptr)
            {
                std::cout << curr->getId() << ", " <<
                       (uf == nullptr ? curr->getPlayer().getGroupId()
                       : uf->debug(curr->getPlayer().getGroupId())) << std::endl;
                ++bucket; curr = curr->getNext();
            }
        }
        return bucket;
    }
    PlayersHashTable() : tableLength(defaultStartingLength), playerCount(0), table(new Node*[tableLength]())
    {}
    PlayersHashTable(const PlayersHashTable& other) = delete;
    PlayersHashTable& operator=(const PlayersHashTable& other) = delete;

    void insert(const Player& player);

    void remove(int playerId);

    const Player& search(int playerId) const;

    bool isMember(int playerId) const;

    ~PlayersHashTable();
};

#endif //PLAYGROUND_PLAYERSHASHTABLE_HPP
