#ifndef B_TREE_TRAITS_H
#define B_TREE_TRAITS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <utility>
#include "BTreeTraitsNode.h"
#include "BTreeTraitsIterator.h"

template <typename T, size_t Order>
class BTreeTraits {
public:
    using node_type = BTreeTraitsNode<T, Order>;
    using node_pointer = node_type*;
    using value_type = typename node_type::value_type;
    using iterator = BTreeTraitsIterator<T, Order>;

    BTreeTraits() : m_root(nullptr), m_size(0) {}

    ~BTreeTraits() {
        delete m_root;
    }

    BTreeTraits(const BTreeTraits& other) : m_root(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        if (other.m_root) {
            m_root = copy_recursive(other.m_root);
            m_size = other.m_size;
        }
    }

    BTreeTraits& operator=(const BTreeTraits& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            delete m_root;
            m_root = other.m_root ? copy_recursive(other.m_root) : nullptr;
            m_size = other.m_size;
        }
        return *this;
    }

    BTreeTraits(BTreeTraits&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = std::exchange(other.m_root, nullptr);
        m_size = std::exchange(other.m_size, 0);
    }

    BTreeTraits& operator=(BTreeTraits&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            delete m_root;
            m_root = std::exchange(other.m_root, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    void insert(const value_type& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_root) {
            m_root = new node_type(true);
            m_root->m_keys.push_back(key);
            m_size = 1;
            return;
        }
        if (m_root->is_full()) {
            node_pointer new_root = new node_type(false);
            new_root->m_children.push_back(m_root);
            split_child(new_root, 0);
            m_root = new_root;
        }
        insert_non_full(m_root, key);
    }

    size_t size() const { return m_size; }

    iterator begin() const { return iterator(m_root); }
    iterator end() const { return iterator(); }

    friend std::ostream& operator<<(std::ostream& os, const BTreeTraits<T, Order>& tree) {
        os << "{ ";
        for(const auto& key : tree) {
            os << key << " ";
        }
        os << "}";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, BTreeTraits<T, Order>& tree) {
        value_type value;
        while (is >> value) {
            tree.insert(value);
        }
        return is;
    }

private:
    mutable std::mutex m_mutex;
    node_pointer m_root;
    size_t m_size;

    void split_child(node_pointer parent, int child_index) {
        node_pointer child_to_split = parent->m_children[child_index];
        node_pointer new_sibling = new node_type(child_to_split->m_is_leaf);
        parent->m_keys.insert(parent->m_keys.begin() + child_index, child_to_split->m_keys[Order - 1]);
        new_sibling->m_keys.assign(child_to_split->m_keys.begin() + Order, child_to_split->m_keys.end());
        child_to_split->m_keys.erase(child_to_split->m_keys.begin() + Order - 1, child_to_split->m_keys.end());
        if (!child_to_split->m_is_leaf) {
            new_sibling->m_children.assign(child_to_split->m_children.begin() + Order, child_to_split->m_children.end());
            child_to_split->m_children.erase(child_to_split->m_children.begin() + Order, child_to_split->m_children.end());
        }
        parent->m_children.insert(parent->m_children.begin() + child_index + 1, new_sibling);
    }

    void insert_non_full(node_pointer node, const value_type& key) {
        auto it = std::upper_bound(node->m_keys.begin(), node->m_keys.end(), key);
        int i = std::distance(node->m_keys.begin(), it);
        if (node->m_is_leaf) {
            node->m_keys.insert(it, key);
            m_size++;
        } else {
            if (node->m_children[i]->is_full()) {
                split_child(node, i);
                if (key > node->m_keys[i]) i++;
            }
            insert_non_full(node->m_children[i], key);
        }
    }

    node_pointer copy_recursive(node_pointer other_node) {
        if (!other_node) return nullptr;
        node_pointer new_node = new node_type(other_node->m_is_leaf);
        new_node->m_keys = other_node->m_keys;
        for (const auto& child : other_node->m_children) {
            new_node->m_children.push_back(copy_recursive(child));
        }
        return new_node;
    }
};

#endif // B_TREE_TRAITS_H
