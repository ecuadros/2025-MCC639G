#ifndef __TRAITS_H__
#define __TRAITS_H__

#include <functional>

// Forward declaration
template <typename Traits>
class CBinaryTreeNode;

// Traits para nodos de árbol binario
template <typename T>
struct BinaryTreeNodeTraits {
    using value_type = T;
};

// Traits para árbol binario completo
template <typename T>
struct BinaryTreeAscTraits {
    using value_type = T;
    using CompareFn = std::less<T>;
    using Node = CBinaryTreeNode<BinaryTreeNodeTraits<T>>;
};

template <typename T>
struct BinaryTreeDescTraits {
    using value_type = T;
    using CompareFn = std::greater<T>;
    using Node = CBinaryTreeNode<BinaryTreeNodeTraits<T>>;
};

#endif // __TRAITS_H__