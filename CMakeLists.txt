cmake_minimum_required(VERSION 3.16)
project(playground)

set(CMAKE_CXX_STANDARD 11)

add_executable(playground library2.cpp main2.cpp Group.cpp GameSystem.hpp GameSystem.cpp SumTreeNode.hpp SumTree.hpp game_exceptions.hpp Player.hpp PlayersHashTable.hpp PlayersHashTable.cpp GroupsUnionFind.hpp GroupsUnionFind.cpp Group.hpp)