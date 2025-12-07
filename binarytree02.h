#ifndef __BINARY_TREE_H__  
#define __BINARY_TREE_H__ 
#include <utility>
#include <algorithm>
#include <cassert>
#include <cstddef>//std::ptrdiff_t,std::size_t
#include <iterator>
#include <fstream>
#include <vector>
#include "types.h"
#include <mutex>//std::scoped_lock
#include "util.h"
using namespace std;


template <typename Traits>
class CBinaryTreeNode{
public:
  // TODO: Change T by KeyNode
  using KeyNode  = typename Traits::KeyNode;
  using value_type = KeyNode;
  using Node = CBinaryTreeNode<Traits>;
  //using Ref = typename Traits::RefType;

private:
    KeyNode       m_data;
    Node *  m_pParent = nullptr;//padre
    Ref     m_ref;
    vector<Node *> m_pChild = {nullptr, nullptr}; // 2 hijos inicializados en nullptr

public:
    CBinaryTreeNode(Node* pParent, KeyNode data, Ref ref, Node* p0 = nullptr, Node* p1 = nullptr)
        : m_pParent(pParent), m_data(data), m_ref(ref)
    {
        m_pChild[0] = p0;
        m_pChild[1] = p1;
    }

    
    // TODO: Keynode 
    KeyNode         getData()                {   return m_data;    }
    KeyNode        &getDataRef()             {   return m_data;    }
 
 // TODO: review if these functions must remain public/private
    //void      setpChild(const Node *pChild, size_t pos)  {   m_pChild[pos] = pChild;  }
    void set_left(Node* pChild) { m_pChild[0] = pChild;}
    void set_right(Node* pChild) {m_pChild[1] = pChild;}
    //Node    * getChild(size_t branch){ return m_pChild[branch];  }
    Node * left() { return m_pChild[0];}
    Node * right() { return m_pChild[1];}
    //Node    *&getChildRef(size_t branch){ return m_pChild[branch];  }
    Node*& left_ref() { return m_pChild[0];}
    Node*& right_ref() { return m_pChild[1];}

    Node    * getParent() { return m_pParent;   }
};


template <typename Container>
class binary_tree_iterator : public general_iterator<Container, class binary_tree_iterator<Container>>
{
public:
    using Parent = class general_iterator<Container, binary_tree_iterator<Container>>;
    using Node   = typename Container::Node;
    using Container = binary_tree_iterator<Container>;

  public:
    binary_tree_iterator(Container *pContainer, Node *pNode) : Parent (pContainer,pNode) {}
    binary_tree_iterator(Container &other)  : Parent (other) {}
    binary_tree_iterator(Container &&other) : Parent(other) {} // Move constructor C++11 en adelante

public:
    // TODO: Fuentes Patrick
    binary_tree_iterator operator++() {
        Parent::m_pNode = Parent::m_pNode ? (Node*)Parent::m_pNode->getpNext() : nullptr;
        return *this;
    }
};
//Done01: ForwardIterator
template <typename Container>
class BinaryTreeForwardIterator{
    public:
        using Node = typename Container::Node;
        using value_type = typename Container::value_type;
        using difference_type = std::ptrdiff_t;//ptrdiff_t(devuelve la resta de dos punteros) cuántos elementos hay entre dos iteradores.
        using ptr = value_type*;
        using reference = value_type&;
        

    private:
        Node* m_current;

        Node* findMinimum(Node* node) {
            //puntero no nullptr y tiene hijo izquierdo entonces descender por el lado izquierdo de BT
            while(node && node->left){
                node = node->left;
            }
            return node;

        }
    
    public:
        BinaryTreeForwardIterator(Node* root = nullptr): m_current(root ? findMinimum(root): nullptr){}
        
        reference operator*() const { return m_current->getDataRef();}
        ptr operator->() const {return &(m_current->getDataRef());}

        BinaryTreeForwardIterator& operator++(){
            if(!m_current){
                return *this;
            }
            //si tiene hijo derecho encontrar el mínimo.
            if(m_current->right){
                m_current = findMinimum(m_current->right)
            }

            else{
                Node* parent = m_current->getParent();
                while (parent && m_current == parent->right)
                {
                    m_current = parent;
                    parent = parent->getParent();
                }
                m_current = parent;
                
            }
            return *this;                
        }
};
//Done02: BackwardIterator
template <typename Container>
class BinaryTreeBackwardIterator
{
    public:
        using Node = typename Container::Node;
        using value_type = typename Container::value_type;
        using difference_type = std::ptrdiff_t;//ptrdiff_t(devuelve la resta de dos punteros) cuántos elementos hay entre dos iteradores.
        using ptr = value_type*;
        using reference = value_type&;
    
    private:
        Node* m_current;
        
        Node* findMaximun(Node* node){
            //mientras puntero no nullptr y tiene hijo derecho entonces descender por el lado derecho
            while (node && node->right)
            {
                node = node->right;
            }
            
            return node;
        }
    public:
        BinaryTreeBackwardIterator(Node* root = nullptr): m_current(root ? findMaximun(root): nullptr){}

        reference operator*() const { return m_current->getDataRef();}
        ptr operator->() const {return &(m_current->getDataRef());}


        BinaryTreeBackwardIterator& operator++(){
            if(!m_current) return *this;

            if(m_current->left){
                m_current = findMaximun(m_current->left);
            }

            else{
                Node* parent = m_current->getParent();
                while (parent && m_current == parent->left){
                    m_current = parent;
                    parent = parent->getParent();
                }
                m_current = parent;
            }
            return *this;
        }

};


template <typename _T>
struct BinaryTreeAscTraits{
    using  T         = _T;
    using  Node      = CBinaryTreeNode<T>;
    using  CompareFn = less<T>;
};

template <typename _T>
struct BinaryTreeDescTraits
{
    using  T         = _T;
    using  Node      = CBinaryTreeNode<T>;
    using  CompareFn = greater<T>;
};

template <typename Traits>
class CBinaryTree{
  public:
    using value_type    = typename Traits::T;
    using Node          = typename Traits::Node;
    
    using CompareFn     = typename Traits::CompareFn;
    using Container     = CBinaryTree<Traits>;
    using iterator      = binary_tree_iterator<Container>;

protected:
    Node    *m_pRoot = nullptr;
    size_t   m_size  = 0;
    CompareFn Compfn;
public: 
    size_t  size()  const       { return m_size;       }
    bool    empty() const       { return size() == 0;  }
    // TODO: insert must receive two paramaters: elem and LinkedValueType value
    virtual void insert(value_type elem, Ref ref) {
        m_pRoot = internal_insert(elem, ref, nullptr, nullptr, m_pRoot);
    }

protected:
    Node* CreateNode(Node* pParent, value_type elem, Ref ref) {
        return new Node(pParent, elem, ref);
    }
    Node* internal_insert(value_type elem, Ref ref, LinkedValueType value,
                          Node* pParent, Node*& rpOrigin)
    {
        if (!rpOrigin) {
            ++m_size;
            return (rpOrigin = CreateNode(pParent, elem, ref));
        }

        size_t branch = Compfn(elem, rpOrigin->getDataRef()) ? 0 : 1;
        return internal_insert(elem, ref, nullptr, rpOrigin, rpOrigin->getChildRef(branch));
        return pNode;
    }
public:

    CBinaryTree(){}
    // TODO: Selis Luis (Copy Constructor)

    //copy constructor
    CBinaryTree(const CBinaryTree &other);

    //move constructor
    CBinaryTree(CBinaryTree &&other);

    // TODO: Selis Luis (Destructor)
    virtual ~CBinaryTree();

    Node* copyTree(Node* pParent, Node pNode){
        if(!pNode) return nullptr;
        Node* newNode = new Node(pParent, pNode->getDataref(), pNode->m_ref);
        newNode->left = copyTree(newNode, pNode->left);
        newNode->right = copyTree(newNode, pNode->right);
        return newNode;

    }
    

    // TODO: generalize this function by using iterators and apply any function
    // Done03: Generalizar inorder, recibe cualquier funcion y parametros extra
    template <typename Function, typename... Args>
     void inorder_variadic(Node* pNode,Function func, Args&&... args){
        if(pNode)
        {
            
            inorder_variadic(pNode->left, func, std::forward<Args>(args)...);
            func(pNode->getDataRef(), std::forward<Args>(args)...);
            inorder_variadic(pNode->right, func, std::forward<Args>(args)...);

        }
     }    
    
    template <typename Function, typename... Args>
    void preorder_variadic(Node* pNode,Function func, Args&&... args)
    {
        if(pNode)
        {    
            
            func(pNode->getDataRef(), std::forward<Args>(args)...);
            preorder_variadic(pNode->left, func, std::forward<Args>(args)...);
            preorder_variadic(pNode->right, func, std::forward<Args>(args)...);
            
        }
    }

    // TODO: Quispe David
 
    // Variadic templates (See foreach.h)
    template <typename Function, typename... Args>
    void postorder_variadic(Node* pNode,Function func, Args&&... args)
    {
        if(pNode)
        {
            
            postorder_variadic(pNode->left, func, std::forward<Args>(args)...);
            postorder_variadic(pNode->right, func, std::forward<Args>(args)...);
            func(pNode->getDataRef(), std::forward<Args>(args)...);

        }
    }

    // TODO: Villanueva Richard
    void preorder (ostream &os)    {   preorder (m_pRoot, os, 0);  }
    // TODO: Generalize this function by using iterators and apply any function
    // Create a new iterator to walk in postorder
    // TODO: Villanueva Richard
/*     void preorder(Node  *pNode, size_t level, ostream &os){
        //foreach(preorderbegin(), preorderend(), fn)
        if( pNode ){   
            os << " --> " << pNode->getDataRef();
            preorder(pNode->getChild(0), level+1, os);
            preorder(pNode->getChild(1), level+1, os);     
        }
    } */

    void print    (ostream &os)    {   print    (m_pRoot, 0, os);  }
    // TODO: generalize this function by using iterators and apply any function
    void print(Node* pNode,size_t level, ostream &os){
        if( pNode ){
            Node *pParent = pNode->getParent();
            print(pNode->right, level+1, os);
            os << string(" | ") * level << pNode->getDataRef() << "(" << (pParent?to_string(pParent->getData()):"Root") << ")" <<endl;    
            print(pNode->right, level+1, os);

        }
    }

    // TODO: Open question for everyone
    // Generalizar el recorrido para recibir cualquier funcion
    // con una cantidad flexible de parametros conm variadic templates
    // https://en.cppreference.com/w/cpp/language/parameter_packs
    
    // TODO: Alcazar Joseph
/*     void postorder(Node  *pNode, size_t level, ostream &os){
        //foreach(postorderbegin(), postorderend(), fn)
        if( pNode ){   
            postorder(pNode->getChild(0), level+1, os);
            postorder(pNode->getChild(1), level+1, os);
            os << " --> " << pNode->getDataRef();
        }
    } */
    template <typename Function, typename... Args>
    void inorder(Function func, Args&&... args) {
    std::scoped_lock lock(m_mutex);
    inorder_variadic(m_pRoot, func, std::forward<Args>(args)...);
    }


    template <typename Function, typename... Args>
    void preorder(Function func, Args&&... args) {
    std::scoped_lock lock(m_mutex);
    preorder_variadic(m_pRoot, func, std::forward<Args>(args)...);
    }


    template <typename Function, typename... Args>
    void postorder(Function func, Args&&... args) {
    std::scoped_lock lock(m_mutex);
    postorder_variadic(m_pRoot, func, std::forward<Args>(args)...);
    }

    template <typename Function, typename... Args>
    
    //Done04: Print usando inorder
    void print_tree(ostream &os) {
    inorder(print(m_pRoot, 0, os));
    }

    

    // TODO: Arriola Aldo
    void Write(ostream &os) { os << *this;  }

    // TODO: Toledo Oscar
    void Read(istream &is)  { /* TODO */  }


};




//Done05: constructor por copia
template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(const CBinaryTree& other)
{
    std::scoped_lock lock(m_mutex, other.m_mutex);
    m_pRoot = copyTree(other.m_pRoot, nullptr);
    m_size = other.m_size;
}

//Done06: move constructor
template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(CBinaryTree &&other)
{
    std::scoped_lock lock(m_mutex, other.m_mutex);
    m_pRoot = std::exchange(other.m_pRoot, nullptr);
    m_size = std::exchange(other.m_size, nullptr);
    Compfn = std::move(other.Compfn);
}

//Done07: destructor
template <typename Traits>
CBinaryTree<Traits>::~CBinaryTree()
{
   
    deleteTree(m_pRoot);    
    m_pRoot = nullptr;
    m_size = 0;
}


// TODO: Arriola Aldo
// operator <<
template <typename Traits>
ostream & operator<<(ostream &os, CBinaryTree<Traits> &obj){
    os << "CBinaryTree with " << obj.size() << " elements.";
    obj.inorder(os);
    return os;
}

// TODO: Toledo Oscar
template <typename Traits>
istream & operator>>(istream &is, CBinaryTree<Traits> &obj){
    // Leer el arbol
    return is;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__