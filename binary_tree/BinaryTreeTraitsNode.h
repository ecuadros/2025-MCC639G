#ifndef INC_2025_MCC639G_BINARYTREETRAITSNODE_H
#define INC_2025_MCC639G_BINARYTREETRAITSNODE_H

#include <utility>

template<typename T>
class BinaryTreeTraitsNode {
public:
    using value_type = T;
    using reference_type = T &;
    using const_reference_type = const T &;
    using pointer_type = T *;
    using const_pointer_type = const T *;
    using node_type = BinaryTreeTraitsNode<T>;
    using node_pointer = node_type *;

    BinaryTreeTraitsNode() noexcept
        : m_value{}, m_left(nullptr), m_right(nullptr), m_parent(nullptr), m_height(1) {
    }

    explicit BinaryTreeTraitsNode(const value_type &value)
        : m_value(value), m_left(nullptr), m_right(nullptr), m_parent(nullptr), m_height(1) {
    }

    explicit BinaryTreeTraitsNode(value_type &&value) noexcept
        : m_value(std::move(value)), m_left(nullptr), m_right(nullptr), m_parent(nullptr), m_height(1) {
    }

    BinaryTreeTraitsNode(const BinaryTreeTraitsNode &) = delete;

    BinaryTreeTraitsNode &operator=(const BinaryTreeTraitsNode &) = delete;

    BinaryTreeTraitsNode(BinaryTreeTraitsNode &&other) noexcept
        : m_value(std::move(other.m_value)),
          m_left(std::exchange(other.m_left, nullptr)),
          m_right(std::exchange(other.m_right, nullptr)),
          m_parent(std::exchange(other.m_parent, nullptr)),
          m_height(std::exchange(other.m_height, 1)) {
    }

    BinaryTreeTraitsNode &operator=(BinaryTreeTraitsNode &&other) noexcept {
        if (this != &other) {
            m_value = std::move(other.m_value);
            m_left = std::exchange(other.m_left, nullptr);
            m_right = std::exchange(other.m_right, nullptr);
            m_parent = std::exchange(other.m_parent, nullptr);
            m_height = std::exchange(other.m_height, 1);
        }
        return *this;
    }

    reference_type value() noexcept { return m_value; }
    const_reference_type value() const noexcept { return m_value; }

    node_pointer left() noexcept { return m_left; }
    const node_pointer left() const noexcept { return m_left; }

    node_pointer right() noexcept { return m_right; }
    const node_pointer right() const noexcept { return m_right; }

    node_pointer parent() noexcept { return m_parent; }
    const node_pointer parent() const noexcept { return m_parent; }

    void set_left(node_pointer left) noexcept {
        m_left = left;
        if (left) left->m_parent = this;
    }

    void set_right(node_pointer right) noexcept {
        m_right = right;
        if (right) right->m_parent = this;
    }

    std::size_t height() const { return m_height; }
    void set_height(std::size_t h) { m_height = h; }

private:
    value_type m_value;
    node_pointer m_left;
    node_pointer m_right;
    node_pointer m_parent;
    std::size_t m_height;
};

#endif //INC_2025_MCC639G_BINARYTREETRAITSNODE_H