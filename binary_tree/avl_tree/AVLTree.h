#ifndef AVL_TREE_TRAITS_H
#define AVL_TREE_TRAITS_H

#include <algorithm>
#include <functional>
#include <utility>
#include "/home/patrick/Documents/study/2025-MCC639G/binary_tree/BaseTree.h"
#include "/home/patrick/Documents/study/2025-MCC639G/binary_tree/BinaryTreeTraitsNode.h"
#include "AVLTreeTraitsIterator.h"

template<typename T>
struct AVLTreeTraits {
    using value_type = T;
    using node_type = BinaryTreeTraitsNode<T>; // Use the unified node
    using iterator_type = AVLTreeTraitsIterator<T, ForwardInOrderPolicy<node_type> >;
    using compare_fn = std::less<T>;
};

template<typename Traits>
class AVLTree : public BaseTree<Traits, AVLTree<Traits> > {
    friend class BaseTree<Traits, AVLTree<Traits> >;
public:
    using Base = BaseTree<Traits, AVLTree<Traits> >;
    using typename Base::node_pointer;
    using typename Base::value_type;

    void insert(value_type val) {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_root = insert_unlocked(this->m_root, std::move(val));
    }

private:
    size_t height(node_pointer node) const { return node ? node->height() : 0; }
    int get_balance(node_pointer node) { return node ? height(node->left()) - height(node->right()) : 0; }

    void update_height(node_pointer node) const {
        if (node) { node->set_height(1 + std::max(height(node->left()), height(node->right()))); }
    }

    node_pointer right_rotate(node_pointer y) {
        node_pointer x = y->left();
        y->set_left(x->right());
        x->set_right(y);
        update_height(y);
        update_height(x);
        return x;
    }

    node_pointer left_rotate(node_pointer x) {
        node_pointer y = x->right();
        x->set_right(y->left());
        y->set_left(x);
        update_height(x);
        update_height(y);
        return y;
    }

    node_pointer insert_unlocked(node_pointer node, value_type val) {
        if (!node) {
            this->m_size++;
            return new typename Base::node_type(std::move(val));
        }
        if (val < node->value()) { node->set_left(insert_unlocked(node->left(), std::move(val))); } else if (
            val > node->value()) { node->set_right(insert_unlocked(node->right(), std::move(val))); } else {
            return node;
        }
        update_height(node);
        int balance = get_balance(node);
        if (balance > 1 && val < node->left()->value()) return right_rotate(node);
        if (balance < -1 && val > node->right()->value()) return left_rotate(node);
        if (balance > 1 && val > node->left()->value()) {
            node->set_left(left_rotate(node->left()));
            return right_rotate(node);
        }
        if (balance < -1 && val < node->right()->value()) {
            node->set_right(right_rotate(node->right()));
            return left_rotate(node);
        }
        return node;
    }

    node_pointer copy_recursive(node_pointer other_node) const {
        if (!other_node) return nullptr;
        node_pointer new_node = new typename Base::node_type(other_node->value());
        new_node->set_left(copy_recursive(other_node->left()));
        new_node->set_right(copy_recursive(other_node->right()));
        update_height(new_node);
        return new_node;
    }
};

#endif // AVL_TREE_TRAITS_H
