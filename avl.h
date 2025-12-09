#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <string>

// ===========================================================================
// CAVLNode: Nodo especializado para árboles AVL
// ===========================================================================
template <typename Traits>
class CAVLNode : public CBinaryTreeNode<Traits> {
 
public:

    using Base = CBinaryTreeNode<Traits>;
    using value_type = typename Traits::T;
    using Ref = long;  
    
protected:
    int m_height = 1;
    
public:
    // CONSTRUCTOR SIMPLE Y DIRECTO
    CAVLNode(typename Traits::Node* pParent, const value_type& elem, Ref ref )
        : Base(pParent, elem, ref), m_height(1) {}
    
    int getHeight() const { return m_height; }
    void setHeight(int h) { m_height = h; }
    
    int getBalance() const {
        const CAVLNode* left = getLeft();
        const CAVLNode* right = getRight();
        int lh = left ? left->getHeight() : 0;
        int rh = right ? right->getHeight() : 0;
        return rh - lh;
    }
    
    const CAVLNode* getLeft() const { 
        return static_cast<const CAVLNode*>(this->m_pChild[0]); 
    }
    
    const CAVLNode* getRight() const { 
        return static_cast<const CAVLNode*>(this->m_pChild[1]); 
    }
    
    CAVLNode* getLeft() { 
        return static_cast<CAVLNode*>(this->getChild(0)); 
    }
    
    CAVLNode* getRight() { 
        return static_cast<CAVLNode*>(this->getChild(1)); 
    }
    
    void setLeft(CAVLNode* n)  { this->setpChild(n, 0); }
    void setRight(CAVLNode* n) { this->setpChild(n, 1); }
};

// ===========================================================================
// Traits para AVL Ascendente
// ===========================================================================
template <typename ValueType>
struct AVLAscTraits {
    using T = ValueType;
    using Node = CAVLNode<AVLAscTraits<ValueType>>;
    using CompareFn = std::less<ValueType>;
};

// ===========================================================================
// Traits para AVL Descendente
// ===========================================================================
template <typename ValueType>
struct AVLDescTraits {
    using T = ValueType;
    using Node = CAVLNode<AVLDescTraits<ValueType>>;
    using CompareFn = std::greater<ValueType>;
};

// ===========================================================================
// CAVLTree: Árbol AVL con auto-balanceo
// ===========================================================================
template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
public:
    using Base = CBinaryTree<Traits>;
    using value_type = typename Traits::T;
    using Node = typename Traits::Node;
    using CompareFn = typename Traits::CompareFn;
    using Container = CAVLTree<Traits>;
    
protected:
    using Ref = long;
    static constexpr CompareFn cmp{};
    
    // ===================================================================
    // Actualizar altura basada en hijos: O(1)
    // ===================================================================
    void updateHeight(Node* n) {
        if (!n) return;
        int lh = n->getLeft() ? n->getLeft()->getHeight() : 0;
        int rh = n->getRight() ? n->getRight()->getHeight() : 0;
        n->setHeight(1 + std::max(lh, rh));
    }

    // ===================================================================
    // Rotación izquierda
    // ===================================================================
    Node* rotateLeft(Node* parent) {
        Node* child = parent->getRight();
        if (!child) return parent;
        
        Node* orphan = child->getLeft();
        
        // Realizar rotación: child sube, parent baja
        child->setLeft(parent);
        parent->setRight(orphan);
        
        // Actualizar parents
        child->setParent(parent->getParent());
        parent->setParent(child);
        if (orphan) orphan->setParent(parent);
        
        // Actualizar alturas (primero parent, luego child)
        updateHeight(parent);
        updateHeight(child);
        
        return child;
    }

    // ===================================================================
    // Rotación derecha
    // ===================================================================
    Node* rotateRight(Node* parent) {
        Node* child = parent->getLeft();
        if (!child) return parent;
        
        Node* orphan = child->getRight();
        
        // Realizar rotación: child sube, parent baja
        child->setRight(parent);
        parent->setLeft(orphan);
        
        // Actualizar parents
        child->setParent(parent->getParent());
        parent->setParent(child);
        if (orphan) orphan->setParent(parent);
        
        // Actualizar alturas (primero parent, luego child)
        updateHeight(parent);
        updateHeight(child);
        
        return child;
    }

    // ===================================================================
    // Balancear nodo después de inserción
    // ===================================================================
    Node* balance(Node* node) {
        if (!node) return nullptr;
        
        int bf = node->getBalance();
        
        // Right-Right o Right-Left
        if (bf > 1) {
            Node* rightChild = node->getRight();
            // Right-Left case: doble rotación
            if (rightChild && rightChild->getBalance() < 0) {
                node->setRight(rotateRight(rightChild));
            }
            return rotateLeft(node);
        }
        
        // Left-Left o Left-Right
        if (bf < -1) {
            Node* leftChild = node->getLeft();
            // Left-Right case: doble rotación
            if (leftChild && leftChild->getBalance() > 0) {
                node->setLeft(rotateLeft(leftChild));
            }
            return rotateRight(node);
        }
        
        return node;
    }

    // ===================================================================
    // internal_insert: Override para agregar balanceo AVL
    // Esta función es llamada por CBinaryTree::insert()
    // ===================================================================
    virtual Node* internal_insert(value_type elem, Ref ref,
                                   Node* pParent, Node*& rpOrigin) override {
        // Caso base: insertar nuevo nodo
        if (!rpOrigin) {
            this->m_size++;
            rpOrigin = new Node(pParent, elem, ref);
            return rpOrigin;
        }
        
        // Inserción recursiva
        size_t branch = cmp(elem, rpOrigin->getDataRef()) ? 0 : 1;
        auto& childRef = rpOrigin->getChildRef(branch);
        Node* child = static_cast<Node*>(childRef);
        
        // Insertar recursivamente - child será actualizado por referencia
        childRef = internal_insert(elem, ref, rpOrigin, child);
        
        // Actualizar parent del hijo (puede haber cambiado por rotación)
        if (childRef) {
            childRef->setParent(rpOrigin);
        }
        
        // Actualizar altura de este nodo
        updateHeight(rpOrigin);
        
        // Balancear y retornar (puede cambiar rpOrigin)
        rpOrigin = balance(rpOrigin);
        
        return rpOrigin;
    }

public:
    CAVLTree() : Base() {}
    
    // Usar insert() heredado de CBinaryTree
    // que llama a nuestro internal_insert() sobrescrito
    
    // ===================================================================
    // printTree: Imprime el árbol de forma visual con balance y altura
    // indent: espacios por nivel de profundidad (default: 4)
    // ===================================================================
    void printTree(int indent = 4, Node* node = nullptr, int depth = 0) const {
        // Primera llamada: inicializar con raíz
        if (depth == 0 && node == nullptr) {
            std::cout << "Arbol AVL (visual):" << std::endl;
            node = static_cast<Node*>(this->m_pRoot);
        }
        
        if (!node) return;
        
        printTree(indent, node->getRight(), depth + 1);
        
        std::cout << std::string(depth * indent, ' ') 
                  << node->getData() 
                  << " (BF:" << node->getBalance() 
                  << " H:" << node->getHeight() << ")"
                  << std::endl;
        
        printTree(indent, node->getLeft(), depth + 1);
    }
    
    // ===================================================================
    // read: Deserializa el árbol desde un stream de entrada
    // ===================================================================
    void read(std::istream& is) {
        value_type val;
        while (is >> val) {
            this->insert(val, 0);
        }
    }
};

#endif // __AVL_H__
