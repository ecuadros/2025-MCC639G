#ifndef AVL_TREE_TRAITS_H
#define AVL_TREE_TRAITS_H

#include <algorithm>
#include <iostream>
#include <mutex>
#include <utility>
#include "AVLTreeTraitsNode.h"
#include "AVLTreeTraitsIterator.h"

template <typename T>
class AVLTreeTraits {
public:
    using node_type = AVLTreeTraitsNode<T>;
    using node_pointer = node_type*;
    using value_type = typename node_type::value_type;
    using iterator = AVLTreeTraitsIterator<T>;

    AVLTreeTraits() : m_root(nullptr), m_size(0) {}

    ~AVLTreeTraits() {
        clear_recursive(m_root);
    }

    AVLTreeTraits(const AVLTreeTraits& other) : m_root(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = copy_recursive(other.m_root);
        m_size = other.m_size;
    }

    AVLTreeTraits& operator=(const AVLTreeTraits& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_recursive(m_root);
            m_root = copy_recursive(other.m_root);
            m_size = other.m_size;
        }
        return *this;
    }

    AVLTreeTraits(AVLTreeTraits&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = std::exchange(other.m_root, nullptr);
        m_size = std::exchange(other.m_size, 0);
    }

    AVLTreeTraits& operator=(AVLTreeTraits&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_recursive(m_root);
            m_root = std::exchange(other.m_root, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    void insert(const value_type& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_root = insert_unlocked(m_root, val);
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size;
    }

    iterator begin() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return iterator(m_root);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    friend std::ostream& operator<<(std::ostream& os, const AVLTreeTraits<T>& tree) {
        os << "{ ";
        for (const auto& value : tree) {
            os << value << " ";
        }
        os << "}";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, AVLTreeTraits<T>& tree) {
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

    size_t height(node_pointer node) { return node ? node->height() : 0; }
    int get_balance(node_pointer node) { return node ? height(node->left()) - height(node->right()) : 0; }
    void update_height(node_pointer node) { if (node) { node->set_height(1 + std::max(height(node->left()), height(node->right()))); } }
    node_pointer right_rotate(node_pointer y) { node_pointer x = y->left(); y->set_left(x->right()); x->set_right(y); update_height(y); update_height(x); return x; }
    node_pointer left_rotate(node_pointer x) { node_pointer y = x->right(); x->set_right(y->left()); y->set_left(x); update_height(x); update_height(y); return y; }

    node_pointer insert_unlocked(node_pointer node, const value_type& val) {
        if (!node) { m_size++; return new node_type(val); }
        if (val < node->value()) { node->set_left(insert_unlocked(node->left(), val)); }
        else if (val > node->value()) { node->set_right(insert_unlocked(node->right(), val)); }
        else { return node; }
        update_height(node);
        int balance = get_balance(node);
        if (balance > 1 && val < node->left()->value()) return right_rotate(node);
        if (balance < -1 && val > node->right()->value()) return left_rotate(node);
        if (balance > 1 && val > node->left()->value()) { node->set_left(left_rotate(node->left())); return right_rotate(node); }
        if (balance < -1 && val < node->right()->value()) { node->set_right(right_rotate(node->right())); return left_rotate(node); }
        return node;
    }

    void clear_recursive(node_pointer node) { if (!node) return; clear_recursive(node->left()); clear_recursive(node->right()); delete node; }
    node_pointer copy_recursive(node_pointer other_node) {
        if (!other_node) return nullptr;
        node_pointer new_node = new node_type(other_node->value());
        new_node->set_left(copy_recursive(other_node->left()));
        new_node->set_right(copy_recursive(other_node->right()));
        update_height(new_node);
        return new_node;
    }
};

#endif // AVL_TREE_TRAITS_H
