#ifndef B_TREE_TRAITS_NODE_H
#define B_TREE_TRAITS_NODE_H

#include <vector>

template <typename T, size_t Order>
class BTreeTraitsNode {
public:
    using value_type = T;
    using node_pointer = BTreeTraitsNode<T, Order>*;

    bool m_is_leaf;
    std::vector<value_type> m_keys;
    std::vector<node_pointer> m_children;

    explicit BTreeTraitsNode(bool is_leaf) : m_is_leaf(is_leaf) {
        m_keys.reserve(2 * Order - 1);
        m_children.reserve(2 * Order);
    }

    ~BTreeTraitsNode() {
        for (node_pointer child : m_children) {
            delete child;
        }
    }

    bool is_full() const {
        return m_keys.size() == 2 * Order - 1;
    }
};

#endif // B_TREE_TRAITS_NODE_H
