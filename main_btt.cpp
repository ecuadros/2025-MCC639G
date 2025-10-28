#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <utility>
#include "binary_tree/BinaryTreeTraits.h"

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

void test_traversals_and_io() {
    std::cout << "\n--- Testing Traversals and I/O ---" << std::endl;
    BinaryTreeTraits<int> tree;
    tree.insert(5); tree.insert(3); tree.insert(7); 
    tree.insert(2); tree.insert(4);

    std::cout << "[In-Order]   "; tree.print_inorder(std::cout); std::cout << std::endl;
    std::cout << "[Pre-Order]  "; tree.print_preorder(std::cout); std::cout << std::endl;
    std::cout << "[Post-Order] "; tree.print_postorder(std::cout); std::cout << std::endl;

    std::stringstream ss("20 30 40 50 60 70 80");
    BinaryTreeTraits<int> tree_from_stream;
    ss >> tree_from_stream;
    std::cout << "\nTree read from stream: " << tree_from_stream << std::endl;
    std::cout << "Tree size (expected 7): " << tree_from_stream.size() << std::endl;

    std::stringstream out_ss;
    out_ss << tree_from_stream;
    std::cout << "Tree written to stream: " << out_ss.str() << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n--- Testing Copy and Move Semantics ---" << std::endl;
    BinaryTreeTraits<int> tree;
    tree.insert(50); tree.insert(30); tree.insert(70);

    // Test Copy Constructor
    std::cout << "\nOriginal tree: " << tree << std::endl;
    BinaryTreeTraits<int> copied_tree = tree;
    std::cout << "Copied (via constructor): " << copied_tree << std::endl;

    // Test Copy Assignment
    BinaryTreeTraits<int> assigned_tree;
    assigned_tree.insert(999);
    std::cout << "Assigned tree (before): " << assigned_tree << std::endl;
    assigned_tree = tree;
    std::cout << "Assigned tree (after):  " << assigned_tree << std::endl;

    // Modify original to prove it's a deep copy
    tree.insert(10);
    std::cout << "\nOriginal tree (modified): " << tree << std::endl;
    std::cout << "Copied tree (should be unchanged): " << copied_tree << std::endl;
    std::cout << "Assigned tree (should be unchanged): " << assigned_tree << std::endl;

    // Test Move Constructor
    BinaryTreeTraits<int> moved_tree = std::move(tree);
    std::cout << "\nMoved tree (from original): " << moved_tree << std::endl;
    std::cout << "Original tree (after move): " << tree << std::endl;
}

// Helper function for the concurrency test
void insert_range(BinaryTreeTraits<int>& tree, int start, int end) {
    for (int i = start; i < end; ++i) {
        tree.insert(i);
    }
}

void test_concurrency() {
    std::cout << "\n--- Concurrency Test ---" << std::endl;
    BinaryTreeTraits<int> concurrent_tree;
    const int num_threads = 4;
    const int num_per_thread = 1000;

    std::vector<std::thread> threads;
    std::cout << "Starting " << num_threads << " threads to insert " 
              << num_per_thread << " elements each..." << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(insert_range, std::ref(concurrent_tree), i * num_per_thread, (i + 1) * num_per_thread);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "All threads finished." << std::endl;
    const size_t expected_size = num_threads * num_per_thread;
    const size_t actual_size = concurrent_tree.size();

    std::cout << "Expected tree size: " << expected_size << std::endl;
    std::cout << "Actual tree size:   " << actual_size << std::endl;

    if (expected_size == actual_size) {
        std::cout << "SUCCESS: The tree size is correct. The mutex is working." << std::endl;
    } else {
        std::cout << "FAILURE: The tree size is incorrect. A race condition likely occurred." << std::endl;
    }
}
