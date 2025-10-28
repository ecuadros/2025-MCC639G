#ifndef AVL_TREE_TRAITS_NODE_H
#define AVL_TREE_TRAITS_NODE_H

#include <cstddef> // For size_t

template <typename T>
class AVLTreeTraitsNode {
public:
    using value_type = T;
    using node_pointer = AVLTreeTraitsNode<T>*;

    explicit AVLTreeTraitsNode(const value_type& val)
        : m_value(val), m_left(nullptr), m_right(nullptr), m_height(1) {}

    value_type& value() { return m_value; }
    const value_type& value() const { return m_value; }

    node_pointer left() const { return m_left; }
    void set_left(node_pointer node) { m_left = node; }

    node_pointer right() const { return m_right; }
    void set_right(node_pointer node) { m_right = node; }

    size_t height() const { return m_height; }
    void set_height(size_t h) { m_height = h; }

private:
    value_type m_value;
    node_pointer m_left;
    node_pointer m_right;
    size_t m_height;
};

#endif // AVL_TREE_TRAITS_NODE_H
