#ifndef BINARY_TREE_TRAITS_ITERATOR_H
#define BINARY_TREE_TRAITS_ITERATOR_H

#include <stack>
#include "BinaryTreeTraitsNode.h"

template<typename T>
class BinaryTreeIteratorBase {
public:
    using node_type = BinaryTreeTraitsNode<T>;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;
    using reference = value_type &;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    reference operator*() const {
        return m_stack.top()->value();
    }

    bool operator!=(const BinaryTreeIteratorBase &other) const {
        if (m_stack.empty() != other.m_stack.empty()) return true;
        if (m_stack.empty()) return false; // both are empty
        return m_stack.top() != other.m_stack.top();
    }

protected:
    BinaryTreeIteratorBase() = default;
    std::stack<node_pointer> m_stack;
};

template<typename T>
class BinaryTreeInOrderIterator : public BinaryTreeIteratorBase<T> {
public:
    using base = BinaryTreeIteratorBase<T>;
    using typename base::node_pointer;

    BinaryTreeInOrderIterator() = default;
    explicit BinaryTreeInOrderIterator(node_pointer root) {
        push_left_path(root);
    }

    BinaryTreeInOrderIterator &operator++() {
        if (!this->m_stack.empty()) {
            node_pointer node = this->m_stack.top();
            this->m_stack.pop();
            push_left_path(node->right());
        }
        return *this;
    }

private:
    void push_left_path(node_pointer node) {
        while (node) {
            this->m_stack.push(node);
            node = node->left();
        }
    }
};

template<typename T>
class BinaryTreePreOrderIterator : public BinaryTreeIteratorBase<T> {
public:
    using base = BinaryTreeIteratorBase<T>;
    using typename base::node_pointer;

    BinaryTreePreOrderIterator() = default;
    explicit BinaryTreePreOrderIterator(node_pointer root) {
        if (root) this->m_stack.push(root);
    }

    BinaryTreePreOrderIterator &operator++() {
        if (!this->m_stack.empty()) {
            node_pointer node = this->m_stack.top();
            this->m_stack.pop();
            if (node->right()) this->m_stack.push(node->right());
            if (node->left()) this->m_stack.push(node->left());
        }
        return *this;
    }
};

template<typename T>
class BinaryTreePostOrderIterator : public BinaryTreeIteratorBase<T> {
public:
    using base = BinaryTreeIteratorBase<T>;
    using typename base::node_pointer;

    BinaryTreePostOrderIterator() = default;
    explicit BinaryTreePostOrderIterator(node_pointer root) {
        push_path_to_first_leaf(root);
    }

    BinaryTreePostOrderIterator &operator++() {
        if (this->m_stack.empty()) return *this;

        node_pointer current = this->m_stack.top();
        this->m_stack.pop();

        if (!this->m_stack.empty()) {
            node_pointer parent = this->m_stack.top();
            if (current == parent->left()) { // We came from the left
                push_path_to_first_leaf(parent->right());
            }
        }
        return *this;
    }
private:
    void push_path_to_first_leaf(node_pointer node) {
        while (node) {
            this->m_stack.push(node);
            if (node->left()) {
                node = node->left();
            } else {
                node = node->right();
            }
        }
    }
};

#endif // BINARY_TREE_TRAITS_ITERATOR_H
