#include <iostream>
#include "SumTree.hpp"
#include "PlayersHashTable.hpp"

int main()
{
    std::cout << "Sum tree:" << std::endl;
    SumTree tree;
    tree.addNode(0);
    tree.addNode(0);
    tree.addNode(3);
    tree.addNode(3);
    tree.addNode(3);
    tree.addNode(4);
    std::cout << tree.getLevelZero() << std::endl;

    std::cout << std::endl << "Hash table:" << std::endl;
    PlayersHashTable table;
    Player player(1,2,3);
    table.insert(player);
    player = Player(11,22,33);
    table.insert(player);
    std::cout << table.isMember(1) << std::endl
        << table.isMember(11) << std::endl
        << table.isMember(111) << std::endl;
    table.remove(11);
    std::cout << "After removal: " << table.isMember(2) << std::endl;
    std::cout << "Deetz of #1: ";
    player = table.search(1);
    std::cout << player.getPlayerId() << ", " << player.getGroupId() << ", " << player.getScore()
        << ", " << player.getLevel() << std::endl;
    return 0;
}
