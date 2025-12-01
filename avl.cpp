#include <iostream>
#include <mutex>
#include <memory>
#include <fstream>
#include "types.h"
#include "binarytree.h"
#include "avl.h"
#include "foreach.h"

void DemoAVL(){
    // values for AVL tree
    AVLTree<int> tree;
    
    // Insert values
    std::vector<std::pair<int, int>> values = {
        {10, 1}, {20, 3}, {5, 6}, {15, 10}, {25, 11}, {3, 31}, {7,2}
    };
    
    for (const auto& par : values) {
        tree.insert(par.first);
    }
    
    std::cout << "Inorder traversal: ";
    tree.traverseInorder([](int value) {
        std::cout << value << " ";
    });
    std::cout << std::endl;
    
    std::cout << "Using foreach with iterators: ";
    foreach(tree.begin(), tree.end(), [](int value) {
        std::cout << value + 1 << " ";
    });
    //foreach(tree.begin(), tree.end(), [](int value) {
    //    std::cout << value +1 << " ";
    //});
/*
    // Recorrido compatible usage:
    tree.traverseInorder([](auto& value) { std::cout <<value << });

    tree.traversePreorder([](auto& value) { std::cout <<value  << });

    tree.traversePostorder([](auto& value) { std::cout <<value  << });

    // Range search for trips between two values
    tree.findInRange(minValue, maxValue, [](const auto& trip) {
        // Process matching trips
    });

    // File operations
    tree.writeToFile("trips.dat");
    tree.readFromFile("trips.dat");
    */
}