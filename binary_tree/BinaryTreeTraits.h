#ifndef INC_2025_MCC639G_BINARYTREETRAITS_H
#define INC_2025_MCC639G_BINARYTREETRAITS_H

#include <ostream>
#include <istream>
#include <mutex>
#include <utility>
#include <iostream>
#include "BinaryTreeTraitsNode.h"

template <typename T>
class BinaryTreeTraits {
public:
    using node_type     = BinaryTreeTraitsNode<T>;
    using node_pointer  = node_type*;
    using value_type    = typename node_type::value_type;

    BinaryTreeTraits() : m_root(nullptr), m_size(0) {}

    BinaryTreeTraits(const BinaryTreeTraits& other)
        : m_root(nullptr), m_size(0)
    {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = copy_recursive(other.m_root);
    }

    BinaryTreeTraits& operator=(const BinaryTreeTraits& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_recursive(m_root);
            m_root = nullptr;
            m_size = 0;
            m_root = copy_recursive(other.m_root);
        }
        return *this;
    }

    BinaryTreeTraits(BinaryTreeTraits&& other) noexcept
        : m_root(std::exchange(other.m_root, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {}

    BinaryTreeTraits& operator=(BinaryTreeTraits&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_recursive(m_root); // Use non-locking clear
            m_root = std::exchange(other.m_root, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    ~BinaryTreeTraits() {
        clear();
    }

    void insert(const value_type& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_root = insert_node_unlocked(m_root, val);
    }

    node_pointer find(const value_type& val) const noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        node_pointer current = m_root;
        while (current) {
            if (val == current->value()) return current;
            current = (val < current->value()) ? current->left() : current->right();
        }
        return nullptr;
    }

    size_t size() const noexcept { return m_size; }

    node_pointer root() const noexcept { return m_root; }

    void clear() noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        clear_recursive(m_root);
        m_root = nullptr;
        m_size = 0;
    }

    void print_inorder(std::ostream& os) const noexcept {
        os << "{ ";
        inorder_unlocked(m_root, os);
        os << "}";
    }

    void print_preorder(std::ostream& os) const noexcept {
        os << "{ ";
        preorder_unlocked(m_root, os);
        os << "}";
    }

    void print_postorder(std::ostream& os) const noexcept {
        os << "{ ";
        postorder_unlocked(m_root, os);
        os << "}";
    }

    friend std::ostream& operator<<(std::ostream& os, const BinaryTreeTraits<T>& tree) {
        std::lock_guard<std::mutex> lock(tree.m_mutex);
        tree.print_inorder(os);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, BinaryTreeTraits<T>& tree) {
        value_type value;
        while (is >> value)
            tree.insert(value);
        return is;
    }

private:
    mutable std::mutex m_mutex;
    node_pointer m_root;
    size_t m_size;

    node_pointer insert_node_unlocked(node_pointer node, const value_type& val) {
        if (!node) {
            ++m_size;
            return new node_type(val);
        }
        if (val < node->value())
            node->set_left(insert_node_unlocked(node->left(), val));
        else if (val > node->value())
            node->set_right(insert_node_unlocked(node->right(), val));
        return node;
    }

    void clear_recursive(node_pointer node) noexcept {
        if (!node) return;
        clear_recursive(node->left());
        clear_recursive(node->right());
        delete node;
    }

    node_pointer copy_recursive(node_pointer other_node) {
        if (!other_node) {
            return nullptr;
        }
        node_pointer new_node = new node_type(other_node->value());
        m_size++;
        new_node->set_left(copy_recursive(other_node->left()));
        new_node->set_right(copy_recursive(other_node->right()));
        return new_node;
    }

    void inorder_unlocked(node_pointer node, std::ostream& os) const noexcept {
        if (!node) return;
        inorder_unlocked(node->left(), os);
        os << node->value() << " ";
        inorder_unlocked(node->right(), os);
    }

    void preorder_unlocked(node_pointer node, std::ostream& os) const noexcept {
        if (!node) return;
        os << node->value() << " ";
        preorder_unlocked(node->left(), os);
        preorder_unlocked(node->right(), os);
    }

    void postorder_unlocked(node_pointer node, std::ostream& os) const noexcept {
        if (!node) return;
        postorder_unlocked(node->left(), os);
        postorder_unlocked(node->right(), os);
        os << node->value() << " ";
    }
};

#endif //INC_2025_MCC639G_BINARYTREETRAITS_H