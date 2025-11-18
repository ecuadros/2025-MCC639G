#ifndef B_TREE_TRAITS_H
#define B_TREE_TRAITS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <utility>
#include "BTreeTraitsNode.h"
#include <functional> // For std::less
#include "BTreeTraitsIterator.h"

template <typename T, size_t TOrder>
struct BTreeTraits {
    using value_type    = T;
    using node_type     = BTreeTraitsNode<T, TOrder>;
    using iterator_type = BTreeTraitsIterator<T, TOrder>;
    using compare_fn    = std::less<T>;
    static constexpr size_t Order = TOrder;
};

template <typename Traits>
class BTree {
public:
    using node_type = typename Traits::node_type;
    using node_pointer = node_type*;
    using value_type = typename Traits::value_type;
    using iterator = typename Traits::iterator_type;
    static constexpr size_t Order = Traits::Order;

    BTree() : m_root(nullptr), m_size(0) {}

    ~BTree() {
        clear_unlocked();
    }

    BTree(const BTree& other) : m_root(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        if (other.m_root) {
            m_root = copy_recursive(other.m_root);
            m_size = other.m_size;
        }
    }

    BTree& operator=(const BTree& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_unlocked();
            m_root = other.m_root ? copy_recursive(other.m_root) : nullptr;
            m_size = other.m_size;
        }
        return *this;
    }

    BTree(BTree&& other) noexcept
        : m_root(std::exchange(other.m_root, nullptr)),
          m_size(std::exchange(other.m_size, 0))
    {}

    BTree& operator=(BTree&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_unlocked();
            m_root = std::exchange(other.m_root, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    void insert(value_type key) {
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
        insert_non_full(m_root, std::move(key));
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        clear_unlocked();
    }

    size_t size() const noexcept { return m_size; }

    iterator begin() const { return iterator(m_root); }
    iterator end() const noexcept { return iterator(); }

    friend std::ostream& operator<<(std::ostream& os, const BTree<Traits>& tree) {
        os << "{ ";
        for(const auto& key : tree) {
            os << key << " ";
        }
        os << "}";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, BTree<Traits>& tree) {
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

    void insert_non_full(node_pointer node, value_type key) {
        auto it = std::upper_bound(node->m_keys.begin(), node->m_keys.end(), key);
        int i = std::distance(node->m_keys.begin(), it);
        if (node->m_is_leaf) {
            node->m_keys.insert(it, std::move(key));
            m_size++;
        } else {
            if (node->m_children[i]->is_full()) {
                split_child(node, i);
                if (key > node->m_keys[i]) i++;
            }
            insert_non_full(node->m_children[i], std::move(key));
        }
    }

    void clear_unlocked() noexcept {
        delete m_root; // Relies on BTreeTraitsNode's recursive destructor
        m_root = nullptr;
        m_size = 0;
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
