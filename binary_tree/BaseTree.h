#ifndef INC_2025_MCC639G_BASETREE_H
#define INC_2025_MCC639G_BASETREE_H

#include <ostream>
#include <istream>
#include <mutex>
#include <utility>
#include <iostream>
#include <functional>

template<typename Traits, typename Derived>
class BaseTree {
public:
    using node_type = typename Traits::node_type;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;
    using iterator = typename Traits::iterator_type;

    BaseTree() : m_root(nullptr), m_size(0) {
    }

    ~BaseTree() {
        clear();
    }

    BaseTree(const BaseTree &other)
        : m_root(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = static_cast<const Derived &>(other).copy_recursive(other.m_root);
        m_size = other.m_size;
    }

    BaseTree &operator=(const BaseTree &other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_unlocked();
            m_root = static_cast<const Derived &>(other).copy_recursive(other.m_root);
            m_size = other.m_size;
        }
        return *static_cast<Derived *>(this);
    }

    BaseTree(BaseTree &&other) noexcept
    {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_root = std::exchange(other.m_root, nullptr);
        m_size = std::exchange(other.m_size, 0);
    }

    BaseTree &operator=(BaseTree &&other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear_unlocked();
            m_root = std::exchange(other.m_root, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *static_cast<Derived *>(this);
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        clear_unlocked();
    }

    template<typename Visitor, typename... Args>
    void preorder_traversal(Visitor visit, Args &&... args) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        preorder_recursive(m_root, visit, std::forward<Args>(args)...);
    }

    template<typename Visitor, typename... Args>
    void inorder_traversal(Visitor visit, Args &&... args) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        inorder_recursive(m_root, visit, std::forward<Args>(args)...);
    }

    template<typename Visitor, typename... Args>
    void postorder_traversal(Visitor visit, Args &&... args) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        postorder_recursive(m_root, visit, std::forward<Args>(args)...);
    }

    void print(std::ostream &os = std::cout) const {
        os << "{ ";
        inorder_traversal([&os](const value_type &val) {
            os << val << " ";
        });
        os << "}";
    }

    size_t size() const noexcept { return m_size; }

    iterator begin() const {
        return iterator(m_root);
    }

    iterator end() const noexcept {
        return iterator();
    }

    friend std::ostream &operator<<(std::ostream &os, const BaseTree &tree) {
        os << "{ ";
        for (const auto &value: static_cast<const Derived &>(tree)) {
            os << value << " ";
        }
        os << "}";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, BaseTree &tree) {
        value_type value;
        while (is >> value) {
            static_cast<Derived &>(tree).insert(value);
        }
        return is;
    }

protected:
    mutable std::mutex m_mutex;
    node_pointer m_root;
    size_t m_size;

    void clear_unlocked() noexcept {
        clear_recursive(m_root);
        m_root = nullptr;
        m_size = 0;
    }

private:
    template<typename Visitor, typename... Args>
    void preorder_recursive(node_pointer node, Visitor &visit, Args &&... args) const {
        if (!node) return;
        visit(node->value(), std::forward<Args>(args)...);
        preorder_recursive(node->left(), visit, std::forward<Args>(args)...);
        preorder_recursive(node->right(), visit, std::forward<Args>(args)...);
    }

    template<typename Visitor, typename... Args>
    void inorder_recursive(node_pointer node, Visitor &visit, Args &&... args) const {
        if (!node) return;
        inorder_recursive(node->left(), visit, std::forward<Args>(args)...);
        visit(node->value(), std::forward<Args>(args)...);
        inorder_recursive(node->right(), visit, std::forward<Args>(args)...);
    }

    template<typename Visitor, typename... Args>
    void postorder_recursive(node_pointer node, Visitor &visit, Args &&... args) const {
        if (!node) return;
        postorder_recursive(node->left(), visit, std::forward<Args>(args)...);
        postorder_recursive(node->right(), visit, std::forward<Args>(args)...);
        visit(node->value(), std::forward<Args>(args)...);
    }

    void clear_recursive(node_pointer node) {
        if (!node) return;
        clear_recursive(node->left());
        clear_recursive(node->right());
        delete node;
    }
};

#endif //INC_2025_MCC639G_BASETREE_H