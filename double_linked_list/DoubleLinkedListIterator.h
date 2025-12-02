#ifndef INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSITERATOR_H
#define INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSITERATOR_H


#include <iterator>
#include "DoubleLinkedListNode.h"

template<typename T>
class DoubleLinkedListIteratorBase {
public:
    using node_type = DoubleLinkedListNode<T>;
    using node_pointer = node_type *;
    using value_type = typename node_type::value_type;
    using reference = typename node_type::reference_type;
    using pointer = value_type *;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    explicit DoubleLinkedListIteratorBase(node_pointer ptr = nullptr) noexcept
        : m_ptr(ptr) {
    }

    reference operator*() const noexcept {
        return m_ptr->value();
    }

    pointer operator->() const noexcept {
        return &m_ptr->value();
    }

    bool operator==(const DoubleLinkedListIteratorBase &other) const noexcept {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const DoubleLinkedListIteratorBase &other) const noexcept {
        return m_ptr != other.m_ptr;
    }

    DoubleLinkedListIteratorBase operator++(int) noexcept {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

protected:
    node_pointer m_ptr;
};

template<typename T>
class DoubleLinkedListForwardIterator : public DoubleLinkedListIteratorBase<T> {
public:
    using base = DoubleLinkedListIteratorBase<T>;
    using base::base; // Inherit constructors

    DoubleLinkedListForwardIterator &operator++() noexcept {
        if (this->m_ptr) this->m_ptr = this->m_ptr->next();
        return *this;
    }
};

template<typename T>
class DoubleLinkedListBackwardIterator : public DoubleLinkedListIteratorBase<T> {
public:
    using base = DoubleLinkedListIteratorBase<T>;
    using base::base; // Inherit constructors

    DoubleLinkedListBackwardIterator &operator++() noexcept {
        if (this->m_ptr) this->m_ptr = this->m_ptr->prev();
        return *this;
    }
};

#endif //INC_2025_MCC639G_DOUBLELINKEDLISTTRAITSITERATOR_H
