#ifndef INC_2025_MCC639G_BINARYTREETRAITS_H
#define INC_2025_MCC639G_BINARYTREETRAITS_H

#include <ostream>
#include <istream>
#include <mutex>
#include <utility>
#include <iostream>
#include <functional>
#include "BinaryTreeTraitsNode.h"
#include "BinaryTreeTraitsIterator.h"

template<typename T>
struct BinaryTreeTraits {
    using value_type = T;
    using node_type = BinaryTreeTraitsNode<T>;
    using compare_fn = std::less<T>;
};

template<typename Traits>
class BinaryTree {
public:
    using node_type = typename Traits::node_type;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;

    // Define forward and reverse iterators using the policies
    using iterator = BinaryTreeTraitsIterator<value_type, ForwardInOrderPolicy<node_type> >;
    using reverse_iterator = BinaryTreeTraitsIterator<value_type, BackwardInOrderPolicy<node_type> >;

    BinaryTree() : m_root(nullptr), m_size(0) {
    }

    ~BinaryTree() {
        clear();
    }

    BinaryTree(const BinaryTree &other)
        : m_root(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = copy_recursive(other.m_root);
        m_size = other.m_size;
    }

    BinaryTree &operator=(const BinaryTree &other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_unlocked();
            m_root = copy_recursive(other.m_root);
            m_size = other.m_size;
        }
        return *this;
    }

    BinaryTree(BinaryTree &&other) noexcept
        : m_root(std::exchange(other.m_root, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {
    }

    BinaryTree &operator=(BinaryTree &&other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_unlocked();
            m_root = std::exchange(other.m_root, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    void insert(const value_type &val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_root = insert_unlocked(m_root, val);
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        clear_unlocked();
    }

    template<typename Visitor>
    void preorder_traversal(Visitor visit) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        preorder_recursive(m_root, visit);
    }

    template<typename Visitor>
    void inorder_traversal(Visitor visit) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        inorder_recursive(m_root, visit);
    }

    template<typename Visitor>
    void postorder_traversal(Visitor visit) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        postorder_recursive(m_root, visit);
    }


    size_t size() const noexcept { return m_size; }

    iterator begin() const {
        return iterator(m_root);
    }

    iterator end() const noexcept {
        return iterator();
    }

    reverse_iterator rbegin() const {
        return reverse_iterator(m_root);
    }

    reverse_iterator rend() const {
        return reverse_iterator();
    }

    friend std::ostream &operator<<(std::ostream &os, const BinaryTree<Traits> &tree) {
        os << "{ ";
        for (const auto &value: tree) {
            os << value << " ";
        }
        os << "}";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, BinaryTree<Traits> &tree) {
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

    node_pointer insert_unlocked(node_pointer node, const value_type &val) {
        if (!node) {
            ++m_size;
            return new node_type(val);
        }
        if (val < node->value()) {
            node->set_left(insert_unlocked(node->left(), val));
        } else if (val > node->value()) {
            node->set_right(insert_unlocked(node->right(), val));
        }
        return node;
    }

    template<typename Visitor>
    void preorder_recursive(node_pointer node, Visitor &visit) const {
        if (!node) return;
        visit(node->value());
        preorder_recursive(node->left(), visit);
        preorder_recursive(node->right(), visit);
    }

    template<typename Visitor>
    void inorder_recursive(node_pointer node, Visitor &visit) const {
        if (!node) return;
        inorder_recursive(node->left(), visit);
        visit(node->value());
        inorder_recursive(node->right(), visit);
    }

    template<typename Visitor>
    void postorder_recursive(node_pointer node, Visitor &visit) const {
        if (!node) return;
        postorder_recursive(node->left(), visit);
        postorder_recursive(node->right(), visit);
        visit(node->value());
    }

    void clear_unlocked() noexcept {
        clear_recursive(m_root);
        m_root = nullptr;
        m_size = 0;
    }

    void clear_recursive(node_pointer node) {
        if (!node) return;
        clear_recursive(node->left());
        clear_recursive(node->right());
        delete node;
    }

    node_pointer copy_recursive(node_pointer other_node) {
        if (!other_node) return nullptr;
        node_pointer new_node = new node_type(other_node->value());
        new_node->set_left(copy_recursive(other_node->left()));
        new_node->set_right(copy_recursive(other_node->right()));
        return new_node;
    }
};

#endif //INC_2025_MCC639G_BINARYTREETRAITS_H