#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

template <typename Traits>
class CAVLNode : public CBinaryTreeNode<value_type>{
public:
  using value_type = typename Traits::T;
  using Node       = CBinaryTreeNode<value_type>;
protected:
    int     m_balanceFactor = 0; // Balance factor for AVL tree
public:
    int getBalance() const { return m_balanceFactor; } // modification
    void setBalance(int bf) { m_balanceFactor = bf; } // modification
};

template <typename _T>
struct AVLAscTraits{
    using  value_type = _T;
    using  Node       = CAVLNode<T>;
    using  CompareFn  = less<T>;
};

template <typename _T>
struct AVLDescTraits{
    using  value_type = _T;
    using  Node       = CAVLNode<T>;
    using  CompareFn  = greater<T>;
};

template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
public:
    using Base       = CBinaryTree<Traits>;
    using Node       = typename Traits::Node;
    using value_type = typename Traits::value_type;  
    using CompareFn  = typename Traits::CompareFn;
    using Container  = CAVLTree<Traits>;
    using iterator   = binary_tree_iterator<Container>;

protected:
    // Additional members for AVL tree balancing can be added here
    // TODO: modificar la insercion para que mantenga
    //       el balance del arbo y realice las 
    //       rotaciones necesarias
    
    Node* rotateLeft(Node* y) {
        Node* x = y->getChild(1);
        Node* T2 = x->getChild(0);

        x->setpChild(y, 0);
        y->setpChild(T2, 1);

        y->setBalance(getHeight(y->getChild(1)) - getHeight(y->getChild(0)));
        x->setBalance(getHeight(x->getChild(1)) - getHeight(x->getChild(0)));

        return x;
    }

    Node* rotateRight(Node* x) {
        Node* y = x->getChild(0);
        Node* T2 = y->getChild(1);

        y->setpChild(x, 1);
        x->setpChild(T2, 0);

        x->setBalance(getHeight(x->getChild(1)) - getHeight(x->getChild(0)));
        y->setBalance(getHeight(y->getChild(1)) - getHeight(y->getChild(0)));

        return y;
    }

    int getHeight(Node* n) {
        if (!n) return 0;
        int lh = getHeight(n->getChild(0));
        int rh = getHeight(n->getChild(1));
        return 1 + std::max(lh, rh);
    }

    int getBalance(Node* n) {
        if (!n) return 0;
        return getHeight(n->getChild(1)) - getHeight(n->getChild(0));
    }
    
    Node *internal_insert(value_type &elem, Ref ref,
                          Node* pParent, Node*& rpOrigin) override
    {
        // TODO 1. insertar
        if (!rpOrigin) {
            Node* newNode = new Node(pParent, elem, ref);
            ++m_size;
            return (rpOrigin = newNode);
        }

        CompareFn cmp;
        size_t branch = cmp(elem, rpOrigin->getDataRef()) ? 0 : 1;
        Node* child = internal_insert(elem, ref, rpOrigin, rpOrigin->getChildRef(branch));
        rpOrigin->setpChild(child, branch);

        // TODO 2. verificar balance
        int bf = getBalance(rpOrigin);
        rpOrigin->setBalance(bf);

        // TODO 3. realizar rotaciones si es necesario
        if (bf > 1) {  // derecha pesada
            if (cmp(elem, rpOrigin->getChild(1)->getDataRef()))
                rpOrigin->setpChild(rotateRight(rpOrigin->getChild(1)), 1);
            return rotateLeft(rpOrigin);
        } else if (bf < -1) {  // izquierda pesada
            if (!cmp(elem, rpOrigin->getChild(0)->getDataRef()))
                rpOrigin->setpChild(rotateLeft(rpOrigin->getChild(0)), 0);
            return rotateRight(rpOrigin);
        }

        return rpOrigin;
    }
public:
    CAVLTree() : Base() {} // Empty tree

};

#endif // __AVL_H__
