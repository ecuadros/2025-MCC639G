#ifndef __BINARY_TREE_H__  
#define __BINARY_TREE_H__ 

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <stack>
#include "types.h"
#include "traits.h"

using namespace std;

// Definición simple de CBinaryTreeNode
template <typename Traits>
class CBinaryTreeNode {
public:
    typedef typename Traits::value_type value_type;
    
private:
    typedef CBinaryTreeNode<Traits> Node;
    
public:
    value_type  m_data;
    Ref         m_ref;
    Node*       m_pParent;
    Node*       m_pLeft;
    Node*       m_pRight;

public:
    CBinaryTreeNode(Node *pParent, value_type data, Ref ref) 
        : m_data(data), m_ref(ref), m_pParent(pParent), m_pLeft(nullptr), m_pRight(nullptr) {}
        
    value_type getData() const { return m_data; }
    value_type& getDataRef() { return m_data; }
    Ref getRef() const { return m_ref; }
    
    Node* getLeft() const { return m_pLeft; }
    Node* getRight() const { return m_pRight; }
    Node* getParent() const { return m_pParent; }
    
    void setLeft(Node* node) { m_pLeft = node; }
    void setRight(Node* node) { m_pRight = node; }
};

// Clase principal CBinaryTree corregida
template <typename Traits>
class CBinaryTree {
public:
    using value_type = typename Traits::value_type;
    using CompareFn = typename Traits::CompareFn;
    using Node = typename Traits::Node;

private:
    Node* m_pRoot;
    size_t m_size;
    CompareFn m_compare;
    mutable mutex m_mutex;

public: 
    CBinaryTree() : m_pRoot(nullptr), m_size(0) {}
    
    // Constructor de copia
    CBinaryTree(const CBinaryTree& other) {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = copyTree(other.m_pRoot, nullptr);
        m_size = other.m_size;
    }
    
    // Operador de asignación
    CBinaryTree& operator=(const CBinaryTree& other) {
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
    CBinaryTree(CBinaryTree&& other) noexcept {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_size = other.m_size;
        other.m_pRoot = nullptr;
        other.m_size = 0;
    }
    
    // Move assignment
    CBinaryTree& operator=(CBinaryTree&& other) noexcept {
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
    
    ~CBinaryTree() {
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
        internalInsert(elem, ref, m_pRoot, nullptr);  
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

private:
    Node* createNode(Node* parent, value_type elem, Ref ref) {
        return new Node(parent, elem, ref);
    }
    
    void internalInsert(value_type elem, Ref ref, Node*& node, Node* parent) {
        if (!node) {
            node = createNode(parent, elem, ref);
            m_size++;
            return;
        }
        
        if (m_compare(elem, node->getData())) {
            internalInsert(elem, ref, node->m_pLeft, node);
        } else {
            internalInsert(elem, ref, node->m_pRight, node);
        }
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
        
        Node* newNode = createNode(parent, other->getData(), other->getRef());
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
            os << string(level * 4, ' ') << node->getData() << "[" << node->getRef() << "]" << endl;
            print(node->getLeft(), os, level + 1);
        }
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, CBinaryTree<Traits>& tree) {
    tree.print(os);
    return os;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__