#ifndef AVL_TREE_TRAITS_ITERATOR_H
#define AVL_TREE_TRAITS_ITERATOR_H

#include <stack>
#include "AVLTreeTraitsNode.h"

template <typename T>
class AVLTreeTraitsIterator {
public:
    using node_type = AVLTreeTraitsNode<T>;
    using node_pointer = node_type*;
    using value_type = typename node_type::value_type;
    using reference = value_type&;
    using pointer = value_type*;

    explicit AVLTreeTraitsIterator(node_pointer root) {
        push_left_path(root);
        m_current = m_stack.empty() ? nullptr : m_stack.top();
    }

    reference operator*() const {
        return m_current->value();
    }

    AVLTreeTraitsIterator& operator++() {
        if (!m_stack.empty()) {
            node_pointer node = m_stack.top();
            m_stack.pop();
            push_left_path(node->right());
            m_current = m_stack.empty() ? nullptr : m_stack.top();
        }
        return *this;
    }

    bool operator!=(const AVLTreeTraitsIterator& other) const {
        return m_current != other.m_current;
    }

private:
    node_pointer m_current;
    std::stack<node_pointer> m_stack;

    void push_left_path(node_pointer node) {
        while (node) {
            m_stack.push(node);
            node = node->left();
        }
    }
};

#endif // AVL_TREE_TRAITS_ITERATOR_H
