#ifndef INC_2025_MCC639G_BINARYTREETRAITS_H
#define INC_2025_MCC639G_BINARYTREETRAITS_H

#include "BaseTree.h"
#include "BinaryTreeTraitsNode.h"
#include "BinaryTreeTraitsIterator.h"

template<typename T>
struct BinaryTreeTraits {
    using value_type = T;
    using node_type = BinaryTreeTraitsNode<T>;
    using iterator_type = BinaryTreeInOrderIterator<T>;
    using compare_fn = std::less<T>;
};

template<typename Traits>
class BinaryTree : public BaseTree<Traits, BinaryTree<Traits> > {
    friend class BaseTree<Traits, BinaryTree<Traits> >;
public:
    using Base = BaseTree<Traits, BinaryTree<Traits> >;
    using typename Base::node_pointer;
    using typename Base::value_type;

    void insert(const value_type &val) {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_root = insert_unlocked(this->m_root, val);
    }

private:
    node_pointer insert_unlocked(node_pointer node, const value_type &val) {
        if (!node) {
            ++this->m_size;
            return new typename Base::node_type(val);
        }
        if (val < node->value()) {
            node->set_left(insert_unlocked(node->left(), val));
        } else if (val > node->value()) {
            node->set_right(insert_unlocked(node->right(), val));
        }
        return node;
    }

    node_pointer copy_recursive(node_pointer other_node) const {
        if (!other_node) return nullptr;
        node_pointer new_node = new typename Base::node_type(other_node->value());
        new_node->set_left(copy_recursive(other_node->left()));
        new_node->set_right(copy_recursive(other_node->right()));
        return new_node;
    }
};

#endif //INC_2025_MCC639G_BINARYTREETRAITS_H