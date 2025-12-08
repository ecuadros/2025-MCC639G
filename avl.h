#ifndef __AVL_H__
#define __AVL_H__

#include <stack>
#include <queue>
#include <fstream>
#include <iostream>
#include "types.h"
#include "binarytree.h"
#include <algorithm>
#include "container.h"

template <typename Traits>
class CAVLNode : public CBinaryTreeNode<Traits>{
public:
    using value_type = typename Traits::T;
    using Ref = typename Traits::Ref;
    using Node       = CAVLNode<Traits>;
    using BaseNode   = CBinaryTreeNode<Traits>;
    // altura par balance
    int m_height;
    
    CAVLNode(Node* pParent, value_type data, Ref ref)
        : CBinaryTreeNode<Traits>(pParent, data, ref), m_height(1){}
    
    // AVLNode(const T1& value, const Ref& ref) 
    //     : data(value), reference(ref), left(nullptr), right(nullptr), height(1), balanceFactor(0) {}
    
    // AVLNode(const T1& value)
    //     : data(value), reference(Ref()), left(nullptr), right(nullptr), height(1), balanceFactor(0) {}
    
    // ~AVLNode() {
    //     delete left;
    //     delete right;
    // }

};

template <typename _T, typename _Ref>
struct AVLAscTraits {
    using T = _T;
    using value_type = _T;
    using Ref = _Ref;
    using Node = CAVLNode<AVLAscTraits<T, Ref>>;
    using CompareFn = std::less<T>;
};

template <typename _T, typename _Ref>
struct AVLDescTraits {
    using T = _T;
    using value_type = _T;
    using Ref = _Ref;
    using Node = CAVLNode<AVLDescTraits<T, Ref>>;
    using CompareFn = std::greater<T>;
};


template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
private:
    using Base = CBinaryTree<Traits>;
    using Node = typename Traits::Node;

    using value_type = typename Traits::value_type;
    using Ref = typename Traits::Ref;
    using CompareFn = typename Traits::CompareFn;
    using Container = CAVLNode<Traits>;

    using forward_iterator = binary_tree_forward_iterator<Container>;
    using backward_iterator = binary_tree_backward_iterator<Container>;

public:
    CAVLTree(): Base(){}


    void insert(value_type data, Ref ref) override {
        std::unique_lock<std::shared_mutex> lock(this->m_mutex);
        //insert AVL
        this->m_pRoot = internal_insert((Node*)this->m_pRoot, data, ref, nullptr);
    }
protected:
    int height(Node* N){
        return (N==nullptr)? 0 : N->m_height;
    }
    
    void updateHeight(Node* N){
        if (N){
            N->m_height = 1+ std::max(height((Node*)N->getChild(0)),
                                        height((Node*)N->getChild(1)));
        }
    }

    int getBalance(Node* N) {
        if (N == nullptr) return 0;
        return height((Node*)N->getChild(0)) - height((Node*)N->getChild(1));
    }

    Node* rightRotate(Node* currentRoot) {
            Node* newRoot = (Node*)currentRoot->getChild(0);
            Node* pivot = (Node*)newRoot->getChild(1);

            // rotate
            newRoot->setpChild(currentRoot, 1); 

            currentRoot->setpChild(pivot, 0);

            // update parents
            if (pivot) pivot->m_pParent = currentRoot;
            
            newRoot->m_pParent = currentRoot->m_pParent; 
            currentRoot->m_pParent = newRoot;            

            if (newRoot->m_pParent) {
                if (newRoot->m_pParent->getChild(1) == currentRoot) 
                    newRoot->m_pParent->setpChild(newRoot, 1);
                else 
                    newRoot->m_pParent->setpChild(newRoot, 0);
            }

            updateHeight(currentRoot);
            updateHeight(newRoot);

            return newRoot; 
        }

    // 
    Node* leftRotate(Node* currentRoot) {
        Node* newRoot = (Node*)currentRoot->getChild(1);
        Node* pivot = (Node*)newRoot->getChild(0);

        // Realizar rotación
        newRoot->setpChild(currentRoot, 0); 
        currentRoot->setpChild(pivot, 1); 

        // Actualizar Padres
        if (pivot) pivot->m_pParent = currentRoot;

        newRoot->m_pParent = currentRoot->m_pParent;
        currentRoot->m_pParent = newRoot;

        if (newRoot->m_pParent) {
            if (newRoot->m_pParent->getChild(1) == currentRoot) 
                newRoot->m_pParent->setpChild(newRoot, 1);
            else 
                newRoot->m_pParent->setpChild(newRoot, 0);
        }

        updateHeight(currentRoot);
        updateHeight(newRoot);

        return newRoot;
    }
private:
    Node* internal_insert(Node* node, value_type data, Ref ref, Node* pParent){
        if (node == nullptr){
            return new Node(pParent, data, ref);
        }
        CompareFn cmp;
        if (cmp(data, node->getData())){
            node->setpChild(internal_insert((Node*)node->getChild(0), data, ref, node), 0);
        }else if(cmp(node->getData(), data)){
            node->setpChild(internal_insert((Node*)node->getChild(1),data,ref, node), 1);
        }else{
            return node;
        }
        updateHeight(node);
        int balance = getBalance(node);
        
        // balance de ramas
        // left-left
        if (balance > 1 && cmp(data, ((Node*)node->getChild(0))->getData())){
            return rightRotate(node);
        }
        // right-right
        if (balance < -1 && cmp( ((Node*)node->getChild(1))->getData(),data)){
            return leftRotate(node);
        }
        // left-right
        if (balance>1 && cmp(((Node*)node->getChild(0))->getData(), data)){
            node->setpChild(leftRotate((Node*)node->getChild(1)), 1);
            return rightRotate(node);
        }
        // right-left
        if(balance <-1 && cmp(data, ((Node*)node->getChild(1))->getData())){
            node->setpChild(rightRotate((Node*)node->getChild(1)), 1);
            return leftRotate(node);
        }
        // sin cambios
        return node;
    }
};


void DemoAVL();
#endif // __AVL_H__