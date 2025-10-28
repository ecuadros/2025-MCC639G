#include <iostream>
#include <sstream>
#include <utility>
#include "double_linked_list/DoubleLinkedListTraits.h"

void test_singly_binary_tree();

int main() {
    std::cout << "--- Running All Test Suites ---" << std::endl;
    test_singly_binary_tree();
    std::cout << "\n--- All tests finished. ---" << std::endl;
    return 0;
}

void test_singly_binary_tree() {
    std::cout << "--- Testing Doubly Linked List (DoubleLinkedListTraits) ---" << std::endl;

    // --- I/O Test ---
    DoubleLinkedListTraits<int> d_list;
    d_list.add(100);
    d_list.add(200);
    d_list.add(300);
    std::cout << "\nInitial list: " << d_list << std::endl;

    // --- Copy Test ---
    std::cout << "\n--- Copy Semantics ---" << std::endl;
    DoubleLinkedListTraits<int> d_copy = d_list;
    d_copy.add(300);
    std::cout << "Original list after copy: " << d_list << std::endl;
    std::cout << "Copied list with new element: " << d_copy << std::endl;

    // --- Move Test ---
    std::cout << "\n--- Move Semantics ---" << std::endl;
    DoubleLinkedListTraits<int> d_moved = std::move(d_copy);
    std::cout << "Moved list: " << d_moved << std::endl;
    std::cout << "Original list (d_copy) after move: " << d_copy << std::endl;

    // --- Range-based for iteration ---
    for (auto& val : d_list)
        std::cout << val << " ";

    std::cout << "\n--- Forward iteration ---" << std::endl;
    for (auto it = d_list.begin(); it != d_list.end(); ++it) {
        std::cout << "Value: " << *it << std::endl;
    }

    std::cout << "\n\n--- Backward iteration ---" << std::endl;
    auto it = d_list.tail();
    while (it) {
        std::cout << "Value: " << it->value() << std::endl;
        it = it->prev();
    }
}
