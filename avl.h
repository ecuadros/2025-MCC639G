#ifndef __AVL_H__
#define __AVL_H__

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <stack>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "types.h"
#include "traits.h"

using namespace std;

template <typename Traits>
class CAVLNode {
public:
    using value_type = typename Traits::value_type;
    using Node = CAVLNode<Traits>;
    
    value_type  m_data;
    Ref         m_ref;
    Node*       m_pParent;
    Node*       m_pLeft;
    Node*       m_pRight;
    int         m_balanceFactor;
    
public:
    CAVLNode(Node *pParent, value_type data, Ref ref) 
        : m_data(data), m_ref(ref), m_pParent(pParent), 
          m_pLeft(nullptr), m_pRight(nullptr), m_balanceFactor(0) {}
    
    value_type getData() const { return m_data; }
    value_type& getDataRef() { return m_data; }
    Ref getRef() const { return m_ref; }
    
    Node* getLeft() const { return m_pLeft; }
    Node* getRight() const { return m_pRight; }
    Node* getParent() const { return m_pParent; }
    
    void setLeft(Node* node) { m_pLeft = node; }
    void setRight(Node* node) { m_pRight = node; }
    void setParent(Node* node) { m_pParent = node; }
    
    int getBalanceFactor() const { return m_balanceFactor; }
    void setBalanceFactor(int bf) { m_balanceFactor = bf; }
};

template <typename T>
struct AVLAscTraits {
    using value_type = T;
    using Node = CAVLNode<AVLAscTraits<T>>;
    using CompareFn = less<T>;
};

template <typename T>
struct AVLDescTraits {
    using value_type = T;
    using Node = CAVLNode<AVLDescTraits<T>>;
    using CompareFn = greater<T>;
};

template <typename Traits>
class CAVLTree {
public:
    using value_type = typename Traits::value_type;
    using CompareFn = typename Traits::CompareFn;
    using Node = typename Traits::Node;

private:
    Node* m_pRoot;
    size_t m_size;
    CompareFn m_compare;
    mutable mutex m_mutex;

    // Calcular altura de un nodo
    int height(Node* node) const {
        if (!node) return 0;
        return 1 + max(height(node->getLeft()), height(node->getRight()));
    }
    
    // Actualizar factor de balance
    void updateBalanceFactor(Node* node) {
        if (node) {
            int leftHeight = height(node->getLeft());
            int rightHeight = height(node->getRight());
            node->setBalanceFactor(leftHeight - rightHeight);
        }
    }
    
    // Rotaciones AVL
    Node* rotateRight(Node* y) {
        Node* x = y->getLeft();
        if (!x) return y;
        
        Node* T2 = x->getRight();
        
        // Realizar rotación
        x->setRight(y);
        y->setLeft(T2);
        
        // Actualizar padres
        if (T2) T2->setParent(y);
        x->setParent(y->getParent());
        y->setParent(x);
        
        // Actualizar factores de balance
        updateBalanceFactor(y);
        updateBalanceFactor(x);
        
        return x;
    }
    
    Node* rotateLeft(Node* x) {
        Node* y = x->getRight();
        if (!y) return x;
        
        Node* T2 = y->getLeft();
        
        // Realizar rotación
        y->setLeft(x);
        x->setRight(T2);
        
        // Actualizar padres
        if (T2) T2->setParent(x);
        y->setParent(x->getParent());
        x->setParent(y);
        
        // Actualizar factores de balance
        updateBalanceFactor(x);
        updateBalanceFactor(y);
        
        return y;
    }
    
    // Balancear el árbol después de inserción
    Node* balance(Node* node, value_type elem) {
        if (!node) return node;
        
        updateBalanceFactor(node);
        int balanceFactor = node->getBalanceFactor();
        
        // Caso Left Left
        if (balanceFactor > 1 && m_compare(elem, node->getLeft()->getData())) {
            return rotateRight(node);
        }
        
        // Caso Right Right
        if (balanceFactor < -1 && !m_compare(elem, node->getRight()->getData())) {
            return rotateLeft(node);
        }
        
        // Caso Left Right
        if (balanceFactor > 1 && !m_compare(elem, node->getLeft()->getData())) {
            node->setLeft(rotateLeft(node->getLeft()));
            return rotateRight(node);
        }
        
        // Caso Right Left
        if (balanceFactor < -1 && m_compare(elem, node->getRight()->getData())) {
            node->setRight(rotateRight(node->getRight()));
            return rotateLeft(node);
        }
        
        return node;
    }
    
    // Inserción con balanceo AVL
    Node* internalInsertAVL(value_type elem, Ref ref, Node*& node, Node* parent) {
        if (!node) {
            node = new Node(parent, elem, ref);
            m_size++;
            return node;
        }
        
        if (m_compare(elem, node->getData())) {
            Node* newLeft = internalInsertAVL(elem, ref, node->m_pLeft, node);
            node->setLeft(newLeft);
        } else {
            Node* newRight = internalInsertAVL(elem, ref, node->m_pRight, node);
            node->setRight(newRight);
        }
        
        return balance(node, elem);
    }
    
    void clearTree(Node* node) {
        if (node) {
            clearTree(node->getLeft());
            clearTree(node->getRight());
            delete node;
        }
    }
    
    Node* copyTree(Node* other, Node* parent) {
        if (!other) return nullptr;
        
        Node* newNode = new Node(parent, other->getData(), other->getRef());
        newNode->setBalanceFactor(other->getBalanceFactor());
        newNode->setLeft(copyTree(other->getLeft(), newNode));
        newNode->setRight(copyTree(other->getRight(), newNode));
        return newNode;
    }
    
    void inorder(Node* node, ostream& os) const {
        if (node) {
            inorder(node->getLeft(), os);
            os << node->getData() << "(" << node->getRef() << ") ";
            inorder(node->getRight(), os);
        }
    }
    
    void preorder(Node* node, ostream& os) const {
        if (node) {
            os << node->getData() << "(" << node->getRef() << ") ";
            preorder(node->getLeft(), os);
            preorder(node->getRight(), os);
        }
    }
    
    void postorder(Node* node, ostream& os) const {
        if (node) {
            postorder(node->getLeft(), os);
            postorder(node->getRight(), os);
            os << node->getData() << "(" << node->getRef() << ") ";
        }
    }
    
    void print(Node* node, ostream& os, size_t level) const {
        if (node) {
            print(node->getRight(), os, level + 1);
            os << string(level * 4, ' ') << node->getData() 
               << "[" << node->getRef() << "]" 
               << "(BF:" << node->getBalanceFactor() << ")" << endl;
            print(node->getLeft(), os, level + 1);
        }
    }
    
    bool checkBalance(Node* node) const {
        if (!node) return true;
        
        int leftHeight = height(node->getLeft());
        int rightHeight = height(node->getRight());
        int balanceFactor = leftHeight - rightHeight;
        
        return (abs(balanceFactor) <= 1) && 
               checkBalance(node->getLeft()) && 
               checkBalance(node->getRight());
    }

public:
    CAVLTree() : m_pRoot(nullptr), m_size(0) {}
    
    // Constructor de copia
    CAVLTree(const CAVLTree& other) {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = copyTree(other.m_pRoot, nullptr);
        m_size = other.m_size;
    }
    
    // Operador de asignación
    CAVLTree& operator=(const CAVLTree& other) {
        if (this != &other) {
            lock_guard<mutex> lock1(m_mutex);
            lock_guard<mutex> lock2(other.m_mutex);
            clear();
            m_pRoot = copyTree(other.m_pRoot, nullptr);
            m_size = other.m_size;
        }
        return *this;
    }
    
    // Move constructor
    CAVLTree(CAVLTree&& other) noexcept {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_size = other.m_size;
        other.m_pRoot = nullptr;
        other.m_size = 0;
    }
    
    // Move assignment
    CAVLTree& operator=(CAVLTree&& other) noexcept {
        if (this != &other) {
            lock_guard<mutex> lock1(m_mutex);
            lock_guard<mutex> lock2(other.m_mutex);
            clear();
            m_pRoot = other.m_pRoot;
            m_size = other.m_size;
            other.m_pRoot = nullptr;
            other.m_size = 0;
        }
        return *this;
    }
    
    ~CAVLTree() {
        clear();
    }
    
    size_t size() const { 
        lock_guard<mutex> lock(m_mutex);
        return m_size; 
    }
    
    bool empty() const { 
        return size() == 0;  
    }
    
    void insert(value_type elem, Ref ref) { 
        lock_guard<mutex> lock(m_mutex);
        m_pRoot = internalInsertAVL(elem, ref, m_pRoot, nullptr);
    }
    
    void clear() {
        lock_guard<mutex> lock(m_mutex);
        clearTree(m_pRoot);
        m_pRoot = nullptr;
        m_size = 0;
    }
    
    void inorder(ostream& os) const {
        lock_guard<mutex> lock(m_mutex);
        inorder(m_pRoot, os);
    }
    
    void preorder(ostream& os) const {
        lock_guard<mutex> lock(m_mutex);
        preorder(m_pRoot, os);
    }
    
    void postorder(ostream& os) const {
        lock_guard<mutex> lock(m_mutex);
        postorder(m_pRoot, os);
    }
    
    void print(ostream& os) const {
        lock_guard<mutex> lock(m_mutex);
        print(m_pRoot, os, 0);
    }
    
    // Verificar si el árbol está balanceado
    bool isBalanced() const {
        lock_guard<mutex> lock(m_mutex);
        return checkBalance(m_pRoot);
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, CAVLTree<Traits>& tree) {
    tree.print(os);
    return os;
}

void DemoAVLTree();

#endif // __AVL_H__