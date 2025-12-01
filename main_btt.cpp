#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <utility>
#include "binary_tree/BinaryTree.h"
#include <cassert>

void test_traversals_and_io();

void test_copy_and_move();

void test_concurrency();

int main() {
    std::cout << "--- BinaryTreeTraits Test Suite ---" << std::endl;
    test_traversals_and_io();
    test_copy_and_move();
    test_concurrency();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

using MyIntTree = BinaryTree<BinaryTreeTraits<int> >;

void test_traversals_and_io() {
    std::cout << "\n--- Testing Traversals and I/O ---" << std::endl;
    MyIntTree tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);

    std::cout << "In-order traversal (via iterator): " << tree << std::endl;

    std::stringstream ss("20 30 40 50 60 70 80");
    MyIntTree tree_from_stream;
    ss >> tree_from_stream;
    std::cout << "\nTree read from stream: " << tree_from_stream << std::endl;
    std::cout << "Tree size (expected 7): " << tree_from_stream.size() << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n--- Testing Copy and Move Semantics ---" << std::endl;
    MyIntTree tree;
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    std::cout << "\nOriginal tree: " << tree << std::endl;
    MyIntTree copied_tree = tree;
    std::cout << "Copied (via constructor): " << copied_tree << std::endl;

    MyIntTree assigned_tree;
    assigned_tree.insert(999);
    std::cout << "Assigned tree (before): " << assigned_tree << std::endl;
    assigned_tree = tree;
    std::cout << "Assigned tree (after):  " << assigned_tree << std::endl;

    tree.insert(10);
    std::cout << "\nOriginal tree (modified): " << tree << std::endl;
    std::cout << "Copied tree (should be unchanged): " << copied_tree << std::endl;

    MyIntTree moved_tree = std::move(tree);
    std::cout << "\nMoved tree (from original): " << moved_tree << std::endl;
    std::cout << "Original tree (after move): " << tree << std::endl;
}

void insert_range(MyIntTree &tree, int start, int end) {
    for (int i = start; i < end; ++i) {
        tree.insert(i);
    }
}

void test_concurrency() {
    std::cout << "\n--- Testing Binary Tree Concurrency ---" << std::endl;
    MyIntTree concurrent_tree;
    const int num_threads = 4;
    const int num_per_thread = 2500;

    std::vector<std::thread> threads;
    std::cout << "Starting " << num_threads << " threads to insert "
            << num_per_thread << " elements each..." << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(insert_range, std::ref(concurrent_tree), i * num_per_thread, (i + 1) * num_per_thread);
    }

    for (auto &t: threads) {
        t.join();
    }

    const size_t expected_size = num_threads * num_per_thread;
    std::cout << "Final tree size: " << concurrent_tree.size() << std::endl;
    assert(concurrent_tree.size() == expected_size);
    std::cout << "Concurrency test passed: Final size is correct." << std::endl;
}