#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include "types.h"

using namespace std;

//
// ===============================================================
//  NODE
// ===============================================================
//
template <typename Traits>
class CBinaryTreeNode{
public:
    using T          = typename Traits::T;
    using value_type = typename Traits::T;
    using Ref        = typename Traits::Ref;

private:
    //typedef CBinaryTreeNode<T> Node;
    using Node = CBinaryTreeNode<Traits>;

public:
    T       m_data;
    Node   *m_pParent = nullptr;
    Ref     m_ref;
    vector<Node *> m_pChild = {nullptr, nullptr}; // 2 hijos inicializados en nullptr

public:
    // TODO: Fuentes Patrick (revisar que el Ref llegue bien)
    CBinaryTreeNode(Node *pParent, T data, Ref ref = nullptr, Node *p0 = nullptr, Node *p1 = nullptr)
        : m_pParent(pParent), m_data(data), m_ref(ref)
    {
        m_pChild[0] = p0;
        m_pChild[1] = p1;
    }

    // Modificación, Constructor copia
    CBinaryTreeNode(const Node &other)
        : m_data(other.m_data), m_ref(other.m_ref), m_pParent(nullptr)
    {
        m_pChild[0] = other.m_pChild[0] ? new Node(*other.m_pChild[0]) : nullptr;
        m_pChild[1] = other.m_pChild[1] ? new Node(*other.m_pChild[1]) : nullptr;
    }

    // TODO: KeyNode
    T         getData()        { return m_data; }
    T        &getDataRef()     { return m_data; }

    // Para que copyTree pueda leer el ref sin violar el private
    Ref       getRef() const   { return m_ref; }

 // TODO: review if these functions must remain public/private
    void      setpChild(const Node *pChild, size_t pos) { m_pChild[pos] = (Node*)pChild; }
    Node     *getChild(size_t branch){ return m_pChild[branch]; }
    Node    *&getChildRef(size_t branch){ return m_pChild[branch]; }
    Node     *getParent(){ return m_pParent; }
};

//
// ===============================================================
//  ITERATOR MACRO
// ===============================================================
//

// TODO: Segura Alex (Eliminar esta macro)
#define _DEF(_Container, _iter)  \
public: \
    typedef class general_iterator<_Container, _iter<_Container> > Parent; \
    typedef typename _Container::Node                             Node;   \
    typedef _iter<_Container>                                     myself;

//
// ===============================================================
//  FORWARD ITERATOR
// ===============================================================
//
template <typename Container>
class binary_tree_iterator : public general_iterator<Container, class binary_tree_iterator<Container> >
{
    _DEF(Container, binary_tree_iterator);

public:
    binary_tree_iterator(Container *pContainer, Node *pNode) : Parent(pContainer, pNode) {}

    binary_tree_iterator(myself &other) : Parent(other) {}
    binary_tree_iterator(myself &&other) : Parent(std::move(other)) {} // Move constructor C++11 en adelante

    // TODO: Fuentes Patrick
    binary_tree_iterator operator++() //Parent::m_pNode = (Node *)Parent::m_pNode->getpNext();  
    {
        // lógica pendiente
        return *this;
    }
};

//
// ===============================================================
//  TRAITS
// ===============================================================
//
template<typename _T>
struct BinaryTreeAscTraits{
    using T         = _T;
    using Node      = CBinaryTreeNode<BinaryTreeAscTraits<_T>>;
    using CompareFn = less<T>;
};

template<typename _T>
struct BinaryTreeDescTraits{
    using T         = _T;
    using Node      = CBinaryTreeNode<BinaryTreeDescTraits<_T>>;
    using CompareFn = greater<T>;
};

//
// ===============================================================
//  BINARY TREE
// ===============================================================
//
template <typename Traits>
class CBinaryTree{
public:
    // TODO: Segura Alex
    typedef typename Traits::T     value_type;
    typedef typename Traits::Node  Node;

    typedef typename Traits::CompareFn      CompareFn;
    typedef CBinaryTree<Traits>             myself;
    typedef binary_tree_iterator<myself>    iterator;

protected:
    Node      *m_pRoot = nullptr;
    size_t     m_size  = 0;
    CompareFn  Compfn;
    mutable std::mutex m_mutex;

public:
    size_t size()  const { return m_size; }
    bool   empty() const { return m_size == 0; }
    // TODO: insert must receive two paramaters: elem and LinkedValueType value
    //
    // =============================================================
    //  INSERT
    // =============================================================
    //
    // Insert concurrente (pattern lock-exchange)
    virtual void insert(value_type &elem, LinkedValueType value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_insert(elem, value, nullptr, m_pRoot);
    }

protected:
    // TODO: Fuentes Patrick
    Node *CreateNode(Node *pParent, value_type &elem, Ref ref){ return new Node(pParent, elem, ref);}
    Node *internal_insert(value_type &elem, Ref ref, LinkedValueType value, Node *pParent, Node *&rpOrigin)
    {
        if (!rpOrigin)
        {
            ++m_size;
            return (rpOrigin = CreateNode(pParent, elem, ref));
        }

        size_t branch = Compfn(elem, rpOrigin->getDataRef());
        return internal_insert(elem, ref, value, rpOrigin,
                               rpOrigin->getChildRef(branch));
    }

public:

    //
    // =============================================================
    //  MOVE CONSTRUCTOR   (versión correcta)
    // =============================================================
    //
      // TODO: Selis Luis (Move Constructor)
    CBinaryTree(CBinaryTree &&other) noexcept
        : m_pRoot(nullptr), m_size(0), Compfn(other.Compfn)
    {
        std::scoped_lock lock(other.m_mutex);

        std::swap(m_pRoot, other.m_pRoot);
        std::swap(m_size,  other.m_size);
    }

    //
    // =============================================================
    //  DESTRUCTOR  (correcto + thread-safe)
    // =============================================================
    //
    // TODO: Selis Luis (Destructor)
    virtual ~CBinaryTree()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        clear(m_pRoot);
        m_pRoot = nullptr;
        m_size  = 0;
    }

    // TODO: Quispe David
    void inorder  (ostream &os)    {   inorder  (m_pRoot, os, 0);  }

    // TODO: Alcazar Joseph
    void postorder(ostream &os)    {   postorder(m_pRoot, os, 0);  }

    // TODO: Villanueva Richard
    void preorder (ostream &os)    {   preorder (m_pRoot, os, 0);  }
    void print    (ostream &os)    {   print    (m_pRoot, os, 0);  }
    void inorder(void (*visit) (value_type& item))
    {   inorder(m_pRoot, visit);    }

protected:

    // TODO: Open question for everyone
    // Generalizar el recorrido para recibir cualquier funcion
    // con una cantidad flexible de parametros conm variadic templates
    // https://en.cppreference.com/w/cpp/language/parameter_packs
    
    // TODO: Quispe David
    void inorder(Node  *pNode, ostream &os, size_t level){
        if( pNode ){
            //Node *pParent = pNode->getParent();
            inorder(pNode->getChild(0), os, level+1);
            os << " --> " << pNode->getDataRef();
            inorder(pNode->getChild(1), os, level+1);
        }
    }

    // TODO: Alcazar Joseph
    void postorder(Node  *pNode, ostream &os, size_t level){
        //foreach(postorderbegin(), postorderend(), fn)
        if( pNode ){   
            postorder(pNode->getChild(0), os, level+1);
            postorder(pNode->getChild(1), os, level+1);
            os << " --> " << pNode->getDataRef();
        }
    }

    // TODO: Generalize this function by using iterators and apply any function
    // Create a new iterator to walk in postorder
    // TODO: Villanueva Richard
    void preorder(Node  *pNode, ostream &os, size_t level){
        //foreach(preorderbegin(), preorderend(), fn)
        if( pNode ){   
            os << " --> " << pNode->getDataRef();
            preorder(pNode->getChild(0), os, level+1);
            preorder(pNode->getChild(1), os, level+1);            
        }
    }
    
    // TODO: generalize this function by using iterators and apply any function
    void print(Node  *pNode, ostream &os, size_t level)
    {
        // foreach(begin(), end(), print);
        if( pNode ){
            Node *pParent = pNode->getParent();
            print(pNode->getChild(1), os, level+1);
            //os << string(" | ") * level << pNode->getDataRef() << "(" << (pParent?(pNode->getBranch()?"R-":"L-") + to_string(pParent->getData()):"Root") << ")" <<endl;
            os << string(" | ") * level << pNode->getDataRef() << "(" << (pParent?to_string(pParent->getData()):"Root") << ")" <<endl;
            print(pNode->getChild(0), os, level+1);
        }
    }

    // TODO: generalize this function by using iterators and apply any function
    // TODO: Quispe David
    void inorder(Node  *pNode, void (*visit) (value_type& item)){
        if( pNode ){   
            inorder(pNode->getChild(0), *visit);
            (*visit)(pNode->getDataRef());
            inorder(pNode->getChild(1), *visit);
        }
    }

    // TODO: Arriola Aldo
    void Write(ostream &os) { os << *this;  }

    // TODO: Toledo Oscar
    void Read(istream &is)  { /* TODO */  }
};

    //
    // =============================================================
    //  CLEAR (recursivo)
    // =============================================================
    //
    void clear(Node *pNode)
    {
        if (pNode)
        {
            clear(pNode->getChild(0));
            clear(pNode->getChild(1));
            delete pNode;
        }
    }

    //
    // =============================================================
    //  COPY TREE
    // =============================================================
    //
    Node *copyTree(Node *newParent, Node *node)
    {
        if (!node) return nullptr;

        Node *newNode = new Node(newParent, node->getDataRef(), node->m_ref);

        newNode->m_pChild[0] = copyTree(newNode, node->getChild(0));
        newNode->m_pChild[1] = copyTree(newNode, node->getChild(1));

        return newNode;
    }
};

//
// ===============================================================
//  OUTPUT OPERATOR
// ===============================================================
template <typename Traits>
ostream & operator<<(ostream &os, CBinaryTree<Traits> &obj){
    os << "CBinaryTree with " << obj.size() << " elements.";
    obj.inorder(os);
    return os;
}

template <typename Traits>
istream & operator>>(istream &is, CBinaryTree<Traits> &obj){
    return is;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__