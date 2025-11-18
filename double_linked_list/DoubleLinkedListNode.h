#ifndef INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSNODE_H
#define INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSNODE_H


#include <utility>

template<typename T>
class DoubleLinkedListNode {
public:
    using value_type = T;
    using reference_type = T &;
    using const_reference_type = const T &;
    using pointer_type = T *;
    using const_pointer_type = const T *;
    using node_type = DoubleLinkedListNode<T>;
    using node_pointer = node_type *;

    DoubleLinkedListNode() noexcept
        : m_value{}, m_next(nullptr), m_prev(nullptr) {
    }

    explicit DoubleLinkedListNode(const value_type &info)
        : m_value(info), m_next(nullptr), m_prev(nullptr) {
    }

    explicit DoubleLinkedListNode(value_type &&info) noexcept
        : m_value(std::move(info)), m_next(nullptr), m_prev(nullptr) {
    }

    DoubleLinkedListNode(const DoubleLinkedListNode &) = delete;

    DoubleLinkedListNode &operator=(const DoubleLinkedListNode &) = delete;

    DoubleLinkedListNode(DoubleLinkedListNode &&other) noexcept
        : m_value(std::move(other.m_value)),
          m_next(std::exchange(other.m_next, nullptr)),
          m_prev(std::exchange(other.m_prev, nullptr)) {
    }

    DoubleLinkedListNode &operator=(DoubleLinkedListNode &&other) noexcept {
        if (this != &other) {
            m_value = std::move(other.m_value);
            m_next = std::exchange(other.m_next, nullptr);
            m_prev = std::exchange(other.m_prev, nullptr);
        }
        return *this;
    }

    reference_type value() noexcept { return m_value; }
    const_reference_type value() const noexcept { return m_value; }

    node_pointer next() noexcept { return m_next; }
    const node_pointer next() const noexcept { return m_next; }
    void set_next(node_pointer next) noexcept { m_next = next; }

    node_pointer prev() noexcept { return m_prev; }
    const node_pointer prev() const noexcept { return m_prev; }
    void set_prev(node_pointer prev) noexcept { m_prev = prev; }

private:
    value_type m_value;
    node_pointer m_next;
    node_pointer m_prev;
};


#endif //INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSNODE_H