#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <utility>
#include "b_tree/BTreeTraits.h"

const size_t B_TREE_ORDER = 3;

void test_traversals_and_io();
void test_copy_and_move();
void test_concurrency();

int main() {
    std::cout << "--- BTreeTraits Test Suite (Order=" << B_TREE_ORDER << ") ---" << std::endl;
    test_traversals_and_io();
    test_copy_and_move();
    test_concurrency();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

void test_traversals_and_io() {
    std::cout << "\n--- Testing Traversals, Splitting, and I/O ---" << std::endl;
    BTreeTraits<int, B_TREE_ORDER> tree;
    
    std::cout << "Inserting 10, 20, 30, 40, 50, 60... (will trigger splits)" << std::endl;
    for(int i = 1; i <= 6; ++i) tree.insert(i * 10);

    std::cout << "In-order traversal (using iterator): " << tree << std::endl;

    std::stringstream ss("5 15 25 35 45");
    BTreeTraits<int, B_TREE_ORDER> tree_from_stream;
    ss >> tree_from_stream;
    std::cout << "Tree read from stream: " << tree_from_stream << std::endl;
    std::cout << "Tree size (expected 5): " << tree_from_stream.size() << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n--- Testing Copy and Move Semantics ---" << std::endl;
    BTreeTraits<int, B_TREE_ORDER> tree;
    tree.insert(50); tree.insert(30); tree.insert(70);

    std::cout << "Original tree: " << tree << std::endl;
    
    BTreeTraits<int, B_TREE_ORDER> copied_tree = tree;
    std::cout << "Copied (via constructor): " << copied_tree << std::endl;

    BTreeTraits<int, B_TREE_ORDER> assigned_tree;
    assigned_tree.insert(999);
    assigned_tree = tree;
    std::cout << "Assigned (via operator=): " << assigned_tree << std::endl;

    tree.insert(10);
    std::cout << "Original tree (modified): " << tree << std::endl;
    std::cout << "Copied tree (should be unchanged): " << copied_tree << std::endl;

    BTreeTraits<int, B_TREE_ORDER> moved_tree = std::move(tree);
    std::cout << "\nMoved tree (from original): " << moved_tree << std::endl;
    std::cout << "Original tree (after move): " << tree << std::endl;
}

// Helper for concurrency test
void insert_range_b_tree(BTreeTraits<int, B_TREE_ORDER>& tree, int start, int end) {
    for (int i = start; i < end; ++i) {
        tree.insert(i);
    }
}

void test_concurrency() {
    std::cout << "\n--- Concurrency Test ---" << std::endl;
    BTreeTraits<int, B_TREE_ORDER> concurrent_tree;
    const int num_threads = 4;
    const int num_per_thread = 100;

    std::vector<std::thread> threads;
    std::cout << "Starting " << num_threads << " threads to insert " 
              << num_per_thread << " elements each..." << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(insert_range_b_tree, std::ref(concurrent_tree), i * num_per_thread, (i + 1) * num_per_thread);
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
