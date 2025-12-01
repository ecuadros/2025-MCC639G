#ifndef __BINARY_TREE_H__  
#define __BINARY_TREE_H__ 
//#include <utility>
//#include <algorithm>
#include <cassert>
#include "types.h"
//#include "util.h"
using namespace std;


template <typename Traits>
class CBinaryTreeNode{
public:
  // TODO: Change T by KeyNode
  // TODO: Segura Alex (typedef -> using)
  //typedef typename Traits::T          value_type;
  using value_type = typename Traits::T;
private:
  //typedef CBinaryTreeNode<T> Node;
  using Node = CBinaryTreeNode<T>;
  public:
    T       m_data;
    Node *  m_pParent = nullptr;
    Ref     m_ref;
    vector<Node *> m_pChild = {nullptr, nullptr}; // 2 hijos inicializados en nullptr
  public:
    // TODO: Fuentes Patrick (revisar que el Ref llegue bien)
    CBinaryTreeNode(Node *pParent, T data, Ref ref = nullptr, Node *p1 = nullptr) 
        : m_pParent(pParent), m_data(data)
    {   m_pChild[0] = p0;   m_pChild[1] = p1;   }

// TODO: Keynode 
    T         getData()                {   return m_data;    }
    T        &getDataRef()             {   return m_data;    }
 
 // TODO: review if these functions must remain public/private
    void      setpChild(const Node *pChild, size_t pos)  {   m_pChild[pos] = pChild;  }
    Node    * getChild(size_t branch){ return m_pChild[branch];  }
    Node    *&getChildRef(size_t branch){ return m_pChild[branch];  }
    Node    * getParent() { return m_pParent;   }
};

// TODO: Segura Alex (Eliminar esta macro)
#define _DEF(_Container, _iter)  \
public: \
    typedef class general_iterator<_Container, _iter<Container> > Parent;     \
    typedef typename _Container::Node                             Node;       \
    typedef _iter<_Container>                                     myself;

template <typename Container>
class binary_tree_iterator : public general_iterator<Container,  class binary_tree_iterator<Container> > // 
{  _DEF(Container, binary_tree_iterator);

  public:
    binary_tree_iterator(Container *pContainer, Node *pNode) : Parent (pContainer,pNode) {}
    binary_tree_iterator(myself &other)  : Parent (other) {}
    binary_tree_iterator(myself &&other) : Parent(other) {} // Move constructor C++11 en adelante

public:
    // TODO: Fuentes Patrick
    binary_tree_iterator operator++() { //Parent::m_pNode = (Node *)Parent::m_pNode->getpNext();  
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
    // TODO: Segura Alex
    typedef typename Traits::T          value_type;
    typedef typename Traits::Node       Node;
    
    typedef typename Traits::CompareFn      CompareFn;
    typedef CBinaryTree<Traits>              myself;
    typedef binary_tree_iterator<myself>    iterator;

protected:
    Node    *m_pRoot = nullptr;
    size_t   m_size  = 0;
    CompareFn Compfn;
public: 
    size_t  size()  const       { return m_size;       }
    bool    empty() const       { return size() == 0;  }
    // TODO: insert must receive two paramaters: elem and LinkedValueType value
    virtual void    insert(value_type &elem, LinkedValueType value) { internal_insert(elem, value, nullptr, m_pRoot);  }

protected:
    // TODO: Fuentes Patrick
    Node *CreateNode(Node *pParent, value_type &elem, Ref ref){ return new Node(pParent, elem); }
    Node *internal_insert(value_type &elem, Ref ref, LinkedValueType value, Node *pParent, Node *&rpOrigin)
    {
        if( !rpOrigin ) //  llegué al fondo de una rama
        {   ++m_size;
            return (rpOrigin = CreateNode(pParent, elem));
        }
        size_t branch = Compfn(elem, rpOrigin->getDataRef() );
        return internal_insert1(elem, ref, rpOrigin, rpOrigin->getChildRef(branch));
    }
public:
    // TODO: Selis Luis (Move Constructor)
    CBinaryTree(Binary &&other){ }

    // TODO: Selis Luis (Destructor)
    virtual ~CBinaryTree(){  } 
    
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

// TODO: Arriola Aldo
// operator <<
template <typename Traits>
ostream & operator<<(ostream &os, CBinaryTree<Traits> &obj){
    os << "CBinaryTree with " << obj.size() << " elements.";
    // Imprimir el larbo inorder
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