#ifndef BINARY_TREE_TRAITS_ITERATOR_H
#define BINARY_TREE_TRAITS_ITERATOR_H

#include <stack>
#include "BinaryTreeTraitsNode.h"

// --- Movement Policies for the Binary Tree Iterator ---

template<typename Node>
struct ForwardInOrderPolicy {
    static void get_initial_path(typename Node::node_pointer node, std::stack<typename Node::node_pointer> &stack) {
        while (node) {
            stack.push(node);
            node = node->left();
        }
    }

    static void advance(std::stack<typename Node::node_pointer> &stack) {
        typename Node::node_pointer node = stack.top();
        stack.pop();
        get_initial_path(node->right(), stack);
    }
};

template<typename Node>
struct BackwardInOrderPolicy {
    static void get_initial_path(typename Node::node_pointer node, std::stack<typename Node::node_pointer> &stack) {
        while (node) {
            stack.push(node);
            node = node->right();
        }
    }

    static void advance(std::stack<typename Node::node_pointer> &stack) {
        typename Node::node_pointer node = stack.top();
        stack.pop();
        get_initial_path(node->left(), stack);
    }
};

template<typename T, typename MovePolicy>
class BinaryTreeTraitsIterator {
public:
    using node_type = BinaryTreeTraitsNode<T>;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;
    using reference = value_type &;

    BinaryTreeTraitsIterator() {
    }

    explicit BinaryTreeTraitsIterator(node_pointer root) {
        MovePolicy::get_initial_path(root, m_stack);
    }

    reference operator*() const {
        return m_stack.top()->value();
    }

    BinaryTreeTraitsIterator &operator++() {
        if (!m_stack.empty())
            MovePolicy::advance(m_stack);
        return *this;
    }

    bool operator!=(const BinaryTreeTraitsIterator &other) const {
        // Iterators are not equal if one has items and the other doesn't,
        // or if their top elements differ.
        if (m_stack.empty() != other.m_stack.empty()) return true;
        if (m_stack.empty()) return false; // both are empty
        return m_stack.top() != other.m_stack.top();
    }

private:
    std::stack<node_pointer> m_stack;
};

#endif // BINARY_TREE_TRAITS_ITERATOR_H
