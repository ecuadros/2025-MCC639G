#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <thread>
#include <cassert>

#include "linked_list/LinkedListTraits.h"

void test_singly_linked_list();
void test_singly_linked_list_concurrency();

int main() {
    std::cout << "--- Running All Test Suites ---" << std::endl;
    test_singly_linked_list();
    test_singly_linked_list_concurrency();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

void test_singly_linked_list() {
    std::cout << "\n\n--- Testing Singly Linked List (LinkedListTraits) ---" << std::endl;

    // I/O Test
    LinkedListTraits<int> s_list;
    s_list.add(10); s_list.add(20);
    std::cout << "Singly list output: " << s_list << std::endl;

    // Copy Test
    LinkedListTraits<int> s_copy = s_list;
    s_copy.add(30);
    std::cout << "Original singly list after copy: " << s_list << std::endl;
    std::cout << "Copied singly list with new element: " << s_copy << std::endl;

    // Move Test
    LinkedListTraits<int> s_moved = std::move(s_copy);
    std::cout << "Moved singly list: " << s_moved << std::endl;
    std::cout << "Original singly list after move: " << s_copy << std::endl;
}

void test_singly_linked_list_concurrency() {
    std::cout << "\n\n--- Testing Singly Linked List Concurrency ---" << std::endl;

    LinkedListTraits<int> shared_list;
    std::vector<std::thread> threads;
    const int num_threads = 10;
    const int items_per_thread = 1000;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&shared_list, items_per_thread]() {
            for (int j = 0; j < items_per_thread; ++j) {
                shared_list.add(j);
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final list size: " << shared_list.size() << std::endl;
    assert(shared_list.size() == num_threads * items_per_thread);
    std::cout << "Concurrency test passed: Final size is correct." << std::endl;
}
