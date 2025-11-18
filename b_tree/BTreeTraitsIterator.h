#ifndef B_TREE_TRAITS_ITERATOR_H
#define B_TREE_TRAITS_ITERATOR_H

#include <stack>
#include "BTreeTraitsNode.h"

// --- Movement Policies for the B-Tree Iterator (In-Order) ---

template<typename Node>
struct ForwardInOrderPolicy {
    using node_pointer = typename Node::node_pointer;
    using PathStack = std::stack<std::pair<node_pointer, size_t> >;

    static void get_initial_path(node_pointer node, PathStack &path) {
        while (node) {
            path.push({node, 0});
            if (node->m_is_leaf) break;
            node = node->m_children[0];
        }
    }

    static void advance(PathStack &path) {
        auto [node, key_idx] = path.top();
        path.pop();

        if (!node->m_is_leaf) {
            get_initial_path(node->m_children[key_idx + 1], path);
        }

        if (key_idx + 1 < node->m_keys.size()) {
            path.push({node, key_idx + 1});
        }
    }
};

template<typename Node>
struct BackwardInOrderPolicy {
    using node_pointer = typename Node::node_pointer;
    using PathStack = std::stack<std::pair<node_pointer, size_t> >;

    static void get_initial_path(node_pointer node, PathStack &path) {
        while (node) {
            path.push({node, node->m_keys.size() - 1});
            if (node->m_is_leaf) break;
            node = node->m_children.back();
        }
    }

    static void advance(PathStack &path) {
        auto [node, key_idx] = path.top();
        path.pop();

        if (!node->m_is_leaf) {
            get_initial_path(node->m_children[key_idx], path);
        }

        if (key_idx > 0) {
            path.push({node, key_idx - 1});
        }
    }
};

template<typename T, size_t Order, typename MovePolicy>
class BTreeTraitsIterator {
public:
    using node_type = BTreeTraitsNode<T, Order>;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;

    BTreeTraitsIterator() = default;

    explicit BTreeTraitsIterator(node_pointer root) {
        if (root) MovePolicy::get_initial_path(root, m_path);
    }

    const value_type &operator*() const {
        return m_path.top().first->m_keys.at(m_path.top().second);
    }

    BTreeTraitsIterator &operator++() {
        if (!m_path.empty()) MovePolicy::advance(m_path);
        return *this;
    }

    bool operator!=(const BTreeTraitsIterator &other) const {
        return m_path.empty() != other.m_path.empty() || (!m_path.empty() && m_path.top() != other.m_path.top());
    }

private:
    std::stack<std::pair<node_pointer, size_t> > m_path;
};

#endif // B_TREE_TRAITS_ITERATOR_H
