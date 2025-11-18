#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <utility>
#include "avl_tree/AVLTree.h"
#include <cassert>

void test_traversals_and_io();

void test_copy_and_move();

void test_concurrency();

int main() {
    std::cout << "--- AVLTreeTraits Test Suite ---" << std::endl;
    test_traversals_and_io();
    test_copy_and_move();
    test_concurrency();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

using MyAVLTree = AVLTree<AVLTreeTraits<int> >;

void test_traversals_and_io() {
    std::cout << "\n--- Testing Traversals, Balancing, and I/O ---" << std::endl;
    MyAVLTree tree;

    std::cout << "Inserting 10, 20, 30, 40, 50..." << std::endl;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);

    std::cout << "In-order traversal (after balancing): " << tree << std::endl;

    std::stringstream ss("5 15 25 35 45");
    MyAVLTree tree_from_stream;
    ss >> tree_from_stream;
    std::cout << "Tree read from stream: " << tree_from_stream << std::endl;
    std::cout << "Tree size (expected 5): " << tree_from_stream.size() << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n--- Testing Copy and Move Semantics ---" << std::endl;
    MyAVLTree tree;
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    std::cout << "Original tree: " << tree << std::endl;

    MyAVLTree copied_tree = tree;
    std::cout << "Copied (via constructor): " << copied_tree << std::endl;

    MyAVLTree assigned_tree;
    assigned_tree.insert(999);
    assigned_tree = tree;
    std::cout << "Assigned (via operator=): " << assigned_tree << std::endl;

    tree.insert(10);
    std::cout << "Original tree (modified): " << tree << std::endl;
    std::cout << "Copied tree (should be unchanged): " << copied_tree << std::endl;

    MyAVLTree moved_tree = std::move(tree);
    std::cout << "\nMoved tree (from original): " << moved_tree << std::endl;
    std::cout << "Original tree (after move): " << tree << std::endl;
}

// Helper for concurrency test
void insert_range_avl(MyAVLTree &tree, int start, int end) {
    for (int i = start; i < end; ++i) {
        tree.insert(i);
    }
}

void test_concurrency() {
    std::cout << "\n--- AVL Tree Concurrency Test ---" << std::endl;
    MyAVLTree concurrent_tree;
    const int num_threads = 4;
    const int num_per_thread = 2500;

    std::vector<std::thread> threads;
    std::cout << "Starting " << num_threads << " threads to insert "
            << num_per_thread << " elements each..." << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(insert_range_avl, std::ref(concurrent_tree), i * num_per_thread, (i + 1) * num_per_thread);
    }

    for (auto &t: threads) {
        t.join();
    }

    const size_t expected_size = num_threads * num_per_thread;
    std::cout << "Final tree size: " << concurrent_tree.size() << std::endl;
    assert(concurrent_tree.size() == expected_size);
    std::cout << "Concurrency test passed: Final size is correct." << std::endl;
}