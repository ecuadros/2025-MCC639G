#ifndef B_TREE_TRAITS_ITERATOR_H
#define B_TREE_TRAITS_ITERATOR_H

#include <stack>
#include "BTreeTraitsNode.h"

template <typename T, size_t Order>
class BTreeTraitsIterator {
public:
    using node_type = BTreeTraitsNode<T, Order>;
    using node_pointer = node_type*;
    using value_type = typename node_type::value_type;

    BTreeTraitsIterator() {}

    explicit BTreeTraitsIterator(node_pointer root) {
        if (root) {
            push_leftmost_path(root);
        }
    }

    const value_type& operator*() const {
        return m_path.top().first->m_keys[m_path.top().second];
    }

    BTreeTraitsIterator& operator++() {
        if (m_path.empty()) {
            return *this;
        }

        auto current_pos = m_path.top();
        m_path.pop();

        node_pointer node = current_pos.first;
        int key_idx = current_pos.second;

        if (key_idx + 1 < node->m_keys.size()) {
            m_path.push({node, key_idx + 1});
        }

        if (!node->m_is_leaf && key_idx < node->m_children.size()) {
            push_leftmost_path(node->m_children[key_idx + 1]);
        }

        return *this;
    }

    bool operator!=(const BTreeTraitsIterator& other) const {
        return !m_path.empty() || !other.m_path.empty();
    }

private:
    std::stack<std::pair<node_pointer, int>> m_path;
    void push_leftmost_path(node_pointer node) {
        while (node) {
            m_path.push({node, 0});
            if (node->m_is_leaf) {
                break;
            }
            node = node->m_children[0];
        }
    }
};

#endif // B_TREE_TRAITS_ITERATOR_H
