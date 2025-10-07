#include <iostream>
#include <fstream>
#include <sstream>
#include <utility> // For std::move

// Include both list implementations
#include "linked_list/LinkedListTraits.h"
#include "double_linked_list/DoubleLinkedListTraits.h"

// Forward declarations for test suites
void test_singly_linked_list();
void test_doubly_linked_list();

/**
 * @brief Main entry point. Runs the test suites for both list types.
 */
int main() {
    std::cout << "--- Running All Test Suites ---" << std::endl;
    test_singly_linked_list();
    test_doubly_linked_list();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

/**
 * @brief Runs a full suite of tests for the Singly Linked List.
 */
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

/**
 * @brief Runs a full suite of tests for the Doubly Linked List.
 */
void test_doubly_linked_list() {
    std::cout << "\n\n--- Testing Doubly Linked List (DoubleLinkedListTraits) ---" << std::endl;

    // I/O Test
    DoubleLinkedListTraits<int> d_list;
    d_list.add(100); d_list.add(200);
    std::cout << "Doubly list output: " << d_list << std::endl;

    // Copy Test
    DoubleLinkedListTraits<int> d_copy = d_list;
    d_copy.add(300);
    std::cout << "Original doubly list after copy: " << d_list << std::endl;
    std::cout << "Copied doubly list with new element: " << d_copy << std::endl;

    // Move Test
    DoubleLinkedListTraits<int> d_moved = std::move(d_copy);
    std::cout << "Moved doubly list: " << d_moved << std::endl;
    std::cout << "Original doubly list after move: " << d_copy << std::endl;

    // Bidirectional Test
    std::cout << "\n--- Bidirectional Iteration Test ---" << std::endl;
    DoubleLinkedListTraits<int> bi_list;
    bi_list.add(1); bi_list.add(2); bi_list.add(3);
    std::cout << "Forward iteration: " << bi_list << std::endl;
    std::cout << "Backward iteration: ";
    auto it = bi_list.end();
    // The end iterator points one past the last element, so we need to decrement first
    if (it != bi_list.begin()) {
        do {
            --it;
            std::cout << *it << " ";
        } while (it != bi_list.begin());
    }
    std::cout << std::endl;
}
