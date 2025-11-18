#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <utility>
#include "double_linked_list/DoubleLinkedList.h"
#include <cassert>

void test_traversals_and_io();

void test_copy_and_move();

void test_concurrency();

int main() {
    std::cout << "--- DoubleLinkedListTraits Test Suite ---" << std::endl;
    test_traversals_and_io();
    test_copy_and_move();
    test_concurrency();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

using MyDoubleList = DoubleLinkedList<DoubleLinkedListTraits<int> >;

void test_traversals_and_io() {
    std::cout << "\n--- Testing Traversals and I/O ---" << std::endl;
    MyDoubleList list;
    list.add(1);
    list.add(2);
    list.add(3);

    std::cout << "Forward traversal (via iterator): " << list << std::endl;

    std::cout << "Backward traversal: ";
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::stringstream ss("10 20 30");
    MyDoubleList list_from_stream;
    ss >> list_from_stream;
    std::cout << "List read from stream: " << list_from_stream << std::endl;
}

void test_copy_and_move() {
    std::cout << "\n--- Testing Copy and Move Semantics ---" << std::endl;
    MyDoubleList list;
    list.add(100);
    list.add(200);

    std::cout << "Original list: " << list << std::endl;

    MyDoubleList copied_list = list;
    std::cout << "Copied (via constructor): " << copied_list << std::endl;

    copied_list.add(300);
    std::cout << "Original list (should be unchanged): " << list << std::endl;
    std::cout << "Copied list (modified): " << copied_list << std::endl;

    MyDoubleList moved_list = std::move(list);
    std::cout << "\nMoved list (from original): " << moved_list << std::endl;
    std::cout << "Original list (after move): " << list << std::endl;
}

// Helper for concurrency test
void test_concurrency() {
    std::cout << "\n--- Testing Doubly Linked List Concurrency ---" << std::endl;

    MyDoubleList concurrent_list;
    const int num_threads = 10;
    const int items_per_thread = 1000;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&concurrent_list, items_per_thread]() {
            for (int j = 0; j < items_per_thread; ++j) {
                concurrent_list.add(j);
            }
        });
    }

    for (auto &t: threads) {
        t.join();
    }

    const size_t expected_size = num_threads * items_per_thread;
    std::cout << "Final list size: " << concurrent_list.size() << std::endl;
    assert(concurrent_list.size() == expected_size);
    std::cout << "Concurrency test passed: Final size is correct." << std::endl;
}