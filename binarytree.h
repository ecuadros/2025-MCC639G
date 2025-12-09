#ifndef __BINARY_TREE_H__  
#define __BINARY_TREE_H__ 

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <functional>

// Definiciones de tipos básicos
using Ref = long;

// ===========================================================================
// CBinaryTreeNode: Nodo base para árboles binarios
// ===========================================================================
template <typename Traits>
class CBinaryTreeNode {
public:
    using value_type = typename Traits::T;
    using Node = typename Traits::Node;

protected:
    value_type m_data;
    Node* m_pParent = nullptr;
    Ref m_ref;
    std::vector<Node*> m_pChild = {nullptr, nullptr};

public:
    CBinaryTreeNode(Node* pParent, value_type data, Ref ref, Node* p0 = nullptr, Node* p1 = nullptr)
        : m_pParent(pParent), m_data(data), m_ref(ref)
    {
        m_pChild[0] = p0;
        m_pChild[1] = p1;
    }

    value_type getData() const { return m_data; }
    value_type& getDataRef() { return m_data; }
    
    void setpChild(Node* pChild, size_t pos) { m_pChild[pos] = pChild; }
    void setParent(Node* pParent) { m_pParent = pParent; }
    
    Node* getChild(size_t branch) { return m_pChild[branch]; }
    Node*& getChildRef(size_t branch) { return m_pChild[branch]; }
    Node* getParent() { return m_pParent; }
};

// ===========================================================================
// Traits por defecto para árboles binarios
// ===========================================================================
template <typename _T>
struct BinaryTreeAscTraits {
    using T = _T;
    using Node = CBinaryTreeNode<BinaryTreeAscTraits<_T>>;
    using CompareFn = std::less<T>;
};

template <typename _T>
struct BinaryTreeDescTraits {
    using T = _T;
    using Node = CBinaryTreeNode<BinaryTreeDescTraits<_T>>;
    using CompareFn = std::greater<T>;
};

// ===========================================================================
// CBinaryTree: Árbol binario de búsqueda base
// ===========================================================================
template <typename Traits>
class CBinaryTree {
public:
    using value_type = typename Traits::T;
    using Node = typename Traits::Node;
    using CompareFn = typename Traits::CompareFn;
    using Container = CBinaryTree<Traits>;

protected:
    Node* m_pRoot = nullptr;
    size_t m_size = 0;
    CompareFn Compfn;

public: 
    size_t size() const { return m_size; }
    bool empty() const { return size() == 0; }
    
    virtual void insert(value_type elem, Ref ref) {
        m_pRoot = internal_insert(elem, ref, nullptr, m_pRoot);
    }

protected:
    virtual Node* CreateNode(Node* pParent, value_type elem, Ref ref) {
        return new Node(pParent, elem, ref);
    }
    
    virtual Node* internal_insert(value_type elem, Ref ref, Node* pParent, Node*& rpOrigin) {
        if (!rpOrigin) {
            ++m_size;
            return (rpOrigin = CreateNode(pParent, elem, ref));
        }

        size_t branch = Compfn(elem, rpOrigin->getDataRef()) ? 0 : 1;
        return internal_insert(elem, ref, rpOrigin, rpOrigin->getChildRef(branch));
    }

public:
    CBinaryTree() : m_pRoot(nullptr), m_size(0) {}
    
    virtual ~CBinaryTree() { 
        // Implementar destrucción recursiva si es necesario
    }
    
    // ===================================================================
    // Recorridos con variadic templates
    // ===================================================================
    
    // INORDER
    template <typename Function, typename... Args>
    void inorder(Function func, Args const&... args) {
        inorder_rec(m_pRoot, 0, func, args...);
    }
    
    // PREORDER
    template <typename Function, typename... Args>
    void preorder(Function func, Args const&... args) {
        preorder_rec(m_pRoot, 0, func, args...);
    }

    // POSTORDER
    template <typename Function, typename... Args>
    void postorder(Function func, Args const&... args) {
        postorder_rec(m_pRoot, 0, func, args...);
    }

private:
    // Funciones recursivas privadas
    template <typename Function, typename... Args>
    void inorder_rec(Node* pNode, size_t level, Function func, Args const&... args) {
        if (pNode) {
            inorder_rec(pNode->getChild(0), level + 1, func, args...);
            func(pNode, level, args...);
            inorder_rec(pNode->getChild(1), level + 1, func, args...);
        }
    }
    
    template <typename Function, typename... Args>
    void preorder_rec(Node* pNode, size_t level, Function func, Args const&... args) {
        if (pNode) {
            func(pNode, level, args...);
            preorder_rec(pNode->getChild(0), level + 1, func, args...);
            preorder_rec(pNode->getChild(1), level + 1, func, args...);
        }
    }
    
    template <typename Function, typename... Args>
    void postorder_rec(Node* pNode, size_t level, Function func, Args const&... args) {
        if (pNode) {
            postorder_rec(pNode->getChild(0), level + 1, func, args...);
            postorder_rec(pNode->getChild(1), level + 1, func, args...);
            func(pNode, level, args...);
        }
    }

public:
    // ===================================================================
    // Método print para visualización estructural
    // ===================================================================
    void print(std::ostream& os) { 
        print(m_pRoot, 0, os); 
    }
    
    void print(Node* pNode, size_t level, std::ostream& os) {
        if (pNode) {
            Node* pParent = pNode->getParent();
            print(pNode->getChild(1), level + 1, os);
            
            // Generar indentación manualmente
            for (size_t i = 0; i < level; ++i) {
                os << " | ";
            }
            
            os << pNode->getDataRef() << "(";
            if (pParent) {
                os << pParent->getData();
            } else {
                os << "Root";
            }
            os << ")" << std::endl;
            
            print(pNode->getChild(0), level + 1, os);
        }
    }

    void Write(std::ostream& os) { 
        os << *this; 
    }

    void Read(std::istream& is) { 
        value_type val;
        while (is >> val) {
            insert(val, 0);
        }
    }
};

// ===========================================================================
// Operadores de stream
// ===========================================================================
template <typename Traits>
std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& obj) {
    os << "CBinaryTree with " << obj.size() << " elements.";
    obj.inorder([&os](auto* node, size_t level) {
        os << " --> " << node->getData();
    });
    return os;
}

template <typename Traits>
std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& obj) {
    obj.Read(is);
    return is;
}

#endif // __BINARY_TREE_H__
