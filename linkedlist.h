#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <functional>
#include <ostream>
#include <utility>
#include <stdexcept>

/* ============================================================
 *  Deducción de tipos desde Traits
 *    - trait_value_type<Trait>::type -> T
 *    - trait_less<Trait, T>::type    -> comparador (por defecto std::less<T>)
 * ============================================================ */
template <typename Trait, typename = void>
class trait_value_type {};

template <typename Trait>
class trait_value_type<Trait, std::void_t<typename Trait::type>> {
    using type = typename Trait::type;
};

template <typename Trait>
class trait_value_type<Trait, std::void_t<typename Trait::ValueType>> {
    using type = typename Trait::ValueType;
};

template <typename Trait>
class trait_value_type<Trait, std::void_t<typename Trait::T>> {
    using type = typename Trait::T;
};

template <template<class> class TraitTmpl, class U>
class trait_value_type<TraitTmpl<U>, void> {
    using type = U;
};

template <typename Trait, typename T, typename = void>
class trait_less { using type = std::less<T>; };

template <typename Trait, typename T>
class trait_less<Trait, T, std::void_t<typename Trait::less>> {
    using type = typename Trait::less;
};

template <typename Trait, typename T>
class trait_less<Trait, T, std::void_t<typename Trait::Less>> {
    using type = typename Trait::Less;
};

template <typename Trait, typename T>
class trait_less<Trait, T, std::void_t<typename Trait::Compare>> {
    using type = typename Trait::Compare;
};

/* ============================================================
 *  TRAITS que pide linkedlist.cpp
 *    LLinkedListAsc<T>  -> orden ascendente (std::less)
 *    LLinkedListDesc<T> -> orden descendente (std::greater)
 * ============================================================ */
template <class T>
class LLinkedListAsc {
    using type = T;
    using less = std::less<T>;
};

template <class T>
class LLinkedListDesc {
    using type = T;
    using less = std::greater<T>;
};

/* ============================================================
 *  Lista simple con Traits
 *    - CLinkedList<Traits>
 *    - Soporta foreach (iteradores) y operator<<
 *    - Insert(value_type&, Ref) (2 parámetros) se mantiene
 * ============================================================ */
template <typename Traits>
class CLinkedList {
public:
    using value_type = typename trait_value_type<Traits>::type;
    using comparator = typename trait_less<Traits, value_type>::type;

private:
    Class Node {
        value_type value;
        Node* next;
        explicit Node(const value_type& v, Node* n=nullptr) : value(v), next(n) {}
    };

    Node* head_{nullptr};
    Node* tail_{nullptr};
    std::size_t size_{0};
    comparator comp_{};

public:
    CLinkedList() = default;
    ~CLinkedList() { clear(); }

    CLinkedList(const CLinkedList&) = delete;
    CLinkedList& operator=(const CLinkedList&) = delete;

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    void clear() noexcept {
        Node* cur = head_;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    /* ------------------------------------------------------------
     * Insert ordenado por Traits (2 parámetros legacy):
     *   - elem: valor a insertar (se almacena)
     *   - ref : referencia/clave auxiliar (compatibilidad; NO se almacena)
     * ------------------------------------------------------------ */
    template <typename Ref>
    void Insert(value_type &elem, Ref /*ref*/) {
        Node* node = new Node(elem);
        if (!head_) { // lista vacía
            head_ = tail_ = node;
            ++size_;
            return;
        }
        // al inicio
        if (comp_(elem, head_->value)) {
            node->next = head_;
            head_ = node;
            ++size_;
            return;
        }
        // posición intermedia/final
        Node* prev = head_;
        Node* cur  = head_->next;
        while (cur && !comp_(elem, cur->value)) {
            prev = cur;
            cur  = cur->next;
        }
        node->next = cur;
        prev->next = node;
        if (!cur) tail_ = node;
        ++size_;
    }

    /* ------------------------------------------------------------
     *  Iteradores para foreach (Forward Iterator)
     * ------------------------------------------------------------ */
    template <bool IsConst>
    class __iterator {
        using node_ptr = std::conditional_t<IsConst, const Node*, Node*>;
        node_ptr ptr_{nullptr};
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = typename CLinkedList::value_type;
        using difference_type   = std::ptrdiff_t;
        using pointer           = std::conditional_t<IsConst, const value_type*, value_type*>;
        using reference         = std::conditional_t<IsConst, const value_type&, value_type&>;

        __iterator() = default;
        explicit __iterator(node_ptr p) : ptr_(p) {}

        // conversión de iterador no-const a const
        template <bool R = IsConst, typename = std::enable_if_t<R>>
        __iterator(const __iterator<false>& it) : ptr_(it.ptr_) {}

        reference operator*()  const { return const_cast<reference>(ptr_->value); }
        pointer   operator->() const { return &const_cast<reference>(ptr_->value); }

        __iterator& operator++()    { if (ptr_) ptr_ = ptr_->next; return *this; }
        __iterator  operator++(int) { __iterator tmp(*this); ++(*this); return tmp; }

        template <bool R>
        bool operator==(const __iterator<R>& other) const { return ptr_ == other.ptr_; }
        template <bool R>
        bool operator!=(const __iterator<R>& other) const { return !(*this == other); }

        template <typename> friend class CLinkedList;
    };

    using iterator = __iterator<false>;
    using const_iterator = __iterator<true>;

    iterator begin() noexcept { return iterator(head_); }
    iterator end()   noexcept { return iterator(nullptr); }
    const_iterator begin() const noexcept { return const_iterator(head_); }
    const_iterator end()   const noexcept { return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept { return const_iterator(head_); }
    const_iterator cend()   const noexcept { return const_iterator(nullptr); }

    value_type& front() {
        if (!head_) throw std::out_of_range("front en lista vacía");
        return head_->value;
    }
    const value_type& front() const {
        if (!head_) throw std::out_of_range("front en lista vacía");
        return head_->value;
    }
};

/* ------------------------------------------------------------
 *  operator<< para imprimir CLinkedList<Traits>
 * ------------------------------------------------------------ */
template <typename Traits>
std::ostream& operator<<(std::ostream& os, CLinkedList<Traits>& obj) {
    bool first = true;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (!first) os << ' ';
        first = false;
        os << *it;
    }
    return os;
}

/* ------------------------------------------------------------
 *  Prototipo para demo (usado por main.cpp)
 * ------------------------------------------------------------ */
void DemoLinkedList();

#endif // LINKEDLIST_H
