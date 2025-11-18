#ifndef INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSITERATOR_H
#define INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSITERATOR_H


#include <iterator>
#include "DoubleLinkedListNode.h"

template <typename T>
class DoubleLinkedListIterator {
public:
    using node_type        = DoubleLinkedListNode<T>;
    using node_pointer     = node_type*;
    using value_type       = typename node_type::value_type;
    using reference        = typename node_type::reference_type;
    using pointer          = value_type*;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type  = std::ptrdiff_t;

    explicit DoubleLinkedListIterator(node_pointer ptr = nullptr) noexcept
        : m_ptr(ptr) {}

    reference operator*() const noexcept {
        return m_ptr->value();
    }

    pointer operator->() const noexcept {
        return &m_ptr->value();
    }

    DoubleLinkedListIterator& operator++() noexcept {
        if (m_ptr) {
            m_ptr = m_ptr->next();
        }
        return *this;
    }

    DoubleLinkedListIterator operator++(int) noexcept {
        DoubleLinkedListIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    DoubleLinkedListIterator& operator--() noexcept {
        if (m_ptr) {
            m_ptr = m_ptr->prev();
        }
        return *this;
    }

    DoubleLinkedListIterator operator--(int) noexcept {
        DoubleLinkedListIterator tmp(*this);
        --(*this);
        return tmp;
    }

    bool operator==(const DoubleLinkedListIterator& other) const noexcept {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const DoubleLinkedListIterator& other) const noexcept {
        return m_ptr != other.m_ptr;
    }

private:
    node_pointer m_ptr;
};


#endif //INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSITERATOR_H