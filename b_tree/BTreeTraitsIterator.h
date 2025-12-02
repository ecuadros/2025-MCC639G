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
class BTreeInOrderIterator : public BTreeIteratorBase<T, Order> {
public:
    using base = BTreeIteratorBase<T, Order>;
    using typename base::node_pointer;
    using typename base::PathStack;

    BTreeInOrderIterator() = default;
    explicit BTreeInOrderIterator(node_pointer root) {
        if (root) push_leftmost_path(root);
    }

    BTreeInOrderIterator &operator++() {
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
class BTreePreOrderIterator : public BTreeIteratorBase<T, Order> {
public:
    using base = BTreeIteratorBase<T, Order>;
    using typename base::node_pointer;

    BTreePreOrderIterator() = default;
    explicit BTreePreOrderIterator(node_pointer root) {
        if (root) this->m_path.push({root, 0});
    }

    BTreePreOrderIterator &operator++() {
        if (this->m_path.empty()) return *this;

        auto [node, key_idx] = this->m_path.top();

        if (!node->m_is_leaf) {
            this->m_path.push({node->m_children[key_idx], 0});
            return *this;
        }

        while (!this->m_path.empty()) {
            auto&[current_node, current_key_idx] = this->m_path.top();
            current_key_idx++;

            if (current_key_idx < current_node->m_keys.size()) {
                return *this;
            }
            this->m_path.pop();
        }
        return *this;
    }
};

template<typename T, size_t Order>
class BTreePostOrderIterator : public BTreeIteratorBase<T, Order> {
public:
    using base = BTreeIteratorBase<T, Order>;
    using typename base::node_pointer;

    BTreePostOrderIterator() = default;
    explicit BTreePostOrderIterator(node_pointer root) {
        if (root) find_first_leaf(root);
    }

    BTreePostOrderIterator &operator++() {
        if (this->m_path.empty()) return *this;

        this->m_path.pop();

        if (this->m_path.empty()) return *this;

        auto&[parent_node, parent_key_idx] = this->m_path.top();

        if (parent_key_idx + 1 < parent_node->m_children.size()) {
            find_first_leaf(parent_node->m_children[parent_key_idx + 1]);
        } else {
            this->m_path.top().second = 0;
        }
        return *this;
    }
private:
    void find_first_leaf(node_pointer node) {
        while (node) {
            this->m_path.push({node, 0});
            if (node->m_is_leaf) break;
            node = node->m_children[0];
        }
    }
};

#endif // B_TREE_TRAITS_ITERATOR_H
