#ifndef INC_2025_MCC639G_DOUBLELINKEDLISTTRAITS_H
#define INC_2025_MCC639G_DOUBLELINKEDLISTTRAITS_H


#include <ostream>
#include <istream>
#include <mutex>
#include <utility>
#include "DoubleLinkedListTraitsNode.h"
#include "DoubleLinkedListTraitsIterator.h"

template <typename T>
class DoubleLinkedListTraits {
public:
    using node_type     = DoubleLinkedListTraitsNode<T>;
    using node_pointer  = node_type*;
    using value_type    = typename node_type::value_type;
    using reference     = typename node_type::reference_type;
    using const_reference = typename node_type::const_reference_type;

    using iterator      = DoubleLinkedListTraitsIterator<T>;

    DoubleLinkedListTraits() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

    DoubleLinkedListTraits(const DoubleLinkedListTraits& other)
        : m_head(nullptr), m_tail(nullptr), m_size(0)
    {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        for (const auto& value : other)
            add(value);
    }

    DoubleLinkedListTraits& operator=(const DoubleLinkedListTraits& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            for (const auto& value : other)
                add(value);
        }
        return *this;
    }

    DoubleLinkedListTraits(DoubleLinkedListTraits&& other) noexcept
        : m_head(std::exchange(other.m_head, nullptr)),
          m_tail(std::exchange(other.m_tail, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {}

    DoubleLinkedListTraits& operator=(DoubleLinkedListTraits&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            m_head = std::exchange(other.m_head, nullptr);
            m_tail = std::exchange(other.m_tail, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    ~DoubleLinkedListTraits() {
        clear();
        std::cout << "[Debug] DoubleLinkedListTraits destructor called." << std::endl;
    }

    void add(value_type info) {
        std::lock_guard<std::mutex> lock(m_mutex);
        node_pointer new_node = new node_type(std::move(info));

        if (!m_head) {
            m_head = new_node;
            m_tail = new_node;
        } else {
            new_node->set_prev(m_tail);
            m_tail->set_next(new_node);
            m_tail = new_node;
        }
        ++m_size;
    }

    iterator begin() const noexcept { return iterator(m_head); }
    iterator end() const noexcept { return iterator(nullptr); }
    iterator rbegin() const noexcept { return iterator(m_tail); }
    iterator rend() const noexcept { return iterator(nullptr); }

    size_t size() const noexcept { return m_size; }

    node_pointer head() const noexcept { return m_head; }
    node_pointer tail() const noexcept { return m_tail; }

    friend std::ostream& operator<<(std::ostream& os, const DoubleLinkedListTraits<T>& list) {
        std::lock_guard<std::mutex> lock(list.m_mutex);
        for (const auto& value : list)
            os << value << " ";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, DoubleLinkedListTraits<T>& list) {
        value_type value;
        while (is >> value)
            list.add(std::move(value));
        return is;
    }

private:
    void clear() noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        node_pointer current = m_head;
        while (current) {
            node_pointer next = current->next();
            delete current;
            current = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    mutable std::mutex m_mutex;
    node_pointer m_head;
    node_pointer m_tail;
    size_t m_size;
};


#endif //INC_2025_MCC639G_DOUBLELINKEDLISTTRAITS_H