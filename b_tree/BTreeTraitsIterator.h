#ifndef B_TREE_TRAITS_ITERATOR_H
#define B_TREE_TRAITS_ITERATOR_H

#include <stack>
#include "BTreeTraitsNode.h"

template<typename T, size_t Order>
class BTreeIteratorBase {
public:
    using node_type = BTreeTraitsNode<T, Order>;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;
    using PathStack = std::stack<std::pair<node_pointer, size_t>>;

    const value_type &operator*() const {
        return m_path.top().first->m_keys.at(m_path.top().second);
    }

    bool operator!=(const BTreeIteratorBase &other) const {
        return m_path.empty() != other.m_path.empty() || (!m_path.empty() && m_path.top() != other.m_path.top());
    }

protected:
    BTreeIteratorBase() = default;
    PathStack m_path;
};

template<typename T, size_t Order>
class BTreeInOrderForwardIterator : public BTreeIteratorBase<T, Order> {
public:
    using base = BTreeIteratorBase<T, Order>;
    using typename base::node_pointer;
    using typename base::PathStack;

    BTreeInOrderForwardIterator() = default;
    explicit BTreeInOrderForwardIterator(node_pointer root) {
        if (root) push_leftmost_path(root);
    }

    BTreeInOrderForwardIterator &operator++() {
        if (this->m_path.empty()) return *this;

        auto [node, key_idx] = this->m_path.top();
        this->m_path.pop();

        if (!node->m_is_leaf) {
            push_leftmost_path(node->m_children[key_idx + 1]);
        }

        if (key_idx + 1 < node->m_keys.size()) {
            this->m_path.push({node, key_idx + 1});
        }
        return *this;
    }

private:
    void push_leftmost_path(node_pointer node) {
        while (node) {
            this->m_path.push({node, 0});
            if (node->m_is_leaf) break;
            node = node->m_children[0];
        }
    }
};

template<typename T, size_t Order>
class BTreeInOrderBackwardIterator : public BTreeIteratorBase<T, Order> {
public:
    using base = BTreeIteratorBase<T, Order>;
    using typename base::node_pointer;
    using typename base::PathStack;

    BTreeInOrderBackwardIterator() = default;
    explicit BTreeInOrderBackwardIterator(node_pointer root) {
        if (root) push_rightmost_path(root);
    }

    BTreeInOrderBackwardIterator &operator++() {
        if (this->m_path.empty()) return *this;

        auto [node, key_idx] = this->m_path.top();
        this->m_path.pop();

        if (!node->m_is_leaf) {
            push_rightmost_path(node->m_children[key_idx]);
        }

        if (key_idx > 0) {
            this->m_path.push({node, key_idx - 1});
        }
        return *this;
    }

private:
    void push_rightmost_path(node_pointer node) {
        while (node) {
            this->m_path.push({node, node->m_keys.size() - 1});
            if (node->m_is_leaf) break;
            node = node->m_children.back();
        }
    }
};

#endif // B_TREE_TRAITS_ITERATOR_H
