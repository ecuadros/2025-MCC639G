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
  using value_type = typename Traits::T;
  using Node = CBinaryTreeNode<T>;

private:
    T       m_data;
    Node *  m_pParent = nullptr;
    Ref     m_ref;
    vector<Node *> m_pChild = {nullptr, nullptr}; // 2 hijos inicializados en nullptr
public:
    CBinaryTreeNode(Node* pParent, KeyNode data, Ref ref, Node* p0 = nullptr, Node* p1 = nullptr)
        : m_pParent(pParent), m_data(data), m_ref(ref), m_pChild(2, nullptr) {
        m_pChild[0] = p0;
        m_pChild[1] = p1;
    }

    value_type detData() const    {   return m_data;    }
    Ref &   getRef()                 {   return m_ref;     }
    
    void septChild(Node *pChild, size_t pos)  {   
        if (pos<2) m_pChild[pos] = pChild;  }
    Node    * getChild(size_t branch){ return (branch<2) m_pChild[branch]: nullptr;  }
    Node    *&getChildRef(size_t branch){ return m_pChild[branch];  }
    Node    * getParent() { return m_pParent;   }
    size_t    getChildCount() const { return m_pChild.size(); }
    
    
    
        // TODO: Keynode 
    T         getData()                {   return m_data;    }
    T        &getDataRef()             {   return m_data;    }
 
 // TODO: review if these functions must remain public/private
    void      setpChild(const Node *pChild, size_t pos)  {   m_pChild[pos] = pChild;  }
    Node    * getChild(size_t branch){ return m_pChild[branch];  }
    Node    *&getChildRef(size_t branch){ return m_pChild[branch];  }
    Node    * getParent() { return m_pParent;   }
};

template <typename Container, bool IsForward = true>
class binary_tree_iterator {   
public:
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;
    using iterator = binary_tree_iterator<Container, IsForward>;

private:
    std::stack<Node*> m_stack;
    Node* m_pCurrent = nullptr;

    // Empuja nodos hacia la izquierda (forward) o derecha (backward)
    void pushToLeaf(Node* node) {
        while (node) {
            m_stack.push(node);
            if constexpr (IsForward)
                node = node->getChild(0);  // Izquierda para forward
            else
                node = node->getChild(1);  // Derecha para backward
        }
    }

public:
    binary_tree_iterator(Node* root = nullptr) {
        if (root) pushToLeaf(root);
        advance();  // Posicionar en el primer elemento
    }

    // Avanza al siguiente nodo en inorder
    void advance() {
        if (m_stack.empty()) {
            m_pCurrent = nullptr;
            return;
        }
        m_pCurrent = m_stack.top();
        m_stack.pop();
        
        // Procesar subárbol opuesto
        if constexpr (IsForward) {
            if (m_pCurrent->getChild(1))
                pushToLeaf(m_pCurrent->getChild(1));
        } else {
            if (m_pCurrent->getChild(0))
                pushToLeaf(m_pCurrent->getChild(0));
        }
    }

    iterator& operator++() {
        advance();
        return *this;
    }

    value_type& operator*() { return m_pCurrent->getDataRef(); }
    
    bool operator==(const iterator& other) const { 
        return m_pCurrent == other.m_pCurrent; 
    }
    bool operator!=(const iterator& other) const { 
        return !(*this == other); 
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
    // ========================================================================
    // ITERADORES: forward (izq->der) y backward (der->izq)
    // ========================================================================
    using forward_iterator  = binary_tree_iterator<Container, true>;
    using backward_iterator = binary_tree_iterator<Container, false>;

protected:
    Node    *m_pRoot = nullptr;
    size_t   m_size  = 0;
    CompareFn Compfn;

// ========================================================================
// CONCURRENCIA: mutex para acceso exclusivo al árbol
// ========================================================================
    mutable std::mutex m_mutex;

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
    }
public:
    // TODO: Selis Luis (Copy Constructor)
    CBinaryTree(Binary &other);
 
    // ========================================================================
    // CONSTRUCTOR COPIA: Transfiere propiedad de recursos
    CBinaryTree(myself &&other): m_pRoot(other.m_pRoot), m_size(other.m_size), Compfn(other.Compfn) { 
        
        std::scoped_lock lock(m_mutex, other.m_mutex);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
        m_size = std::exchange(other.m_size, 0);
        Compfn = std::exchange(other.Compfn, nullptr);
        
        }

    // ========================================================================
    // MOVE CONSTRUCTOR: Transfiere propiedad de recursos
    // Invalida el objeto origen para evitar double-free
   
    CBinaryTree(miself &&other) {
        std::scoped_lock lock(m_mutex, other.m_mutex);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
        m_size = std::exchange(other.m_size, 0);
    }

    //operador de asignación
    myself & operator=(myself &&other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear(m_pRoot);
            m_pRoot = std::exchange(other.m_pRoot, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }


    
    // TODO: Selis Luis (Destructor)
    virtual ~CBinaryTree(){
        std::lock_guard<std::mutex> lock(m_mutex);
        clear();
        m_pRoot = nullptr;
        m_size = 0;
      } 
    
    void clear(Node *pNode) {
        std::lock_guard<std::mutex> lock(m_mutex);
        clearTree(m_pRoot);
        m_pRoot = nullptr;
        m_size = 0;
        }
    
    size_t size() { 
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size; 
        }

    bool empty() { 
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size == 0;  
        }

    // ========================================================================
    // ITERADORES: begin/end para forward y backward
    // ========================================================================
    forward_iterator begin() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return forward_iterator(m_pRoot);
    }
    forward_iterator end() { return forward_iterator(nullptr); }
    
    backward_iterator rbegin() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return backward_iterator(m_pRoot);
    }
    backward_iterator rend() { return backward_iterator(nullptr); }


    // TODO: Quispe David
    // ========================================================================
    // INORDER VARIADIC: Permite pasar múltiples funciones/argumentos
    // Cada función recibe (Node*, level, args...)
    // ========================================================================
    template <typename Function, typename... Args>
    void inorder(Function func, Args const&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_inorder(m_pRoot, 0, func, args...);
    }
    
    template <typename Function, typename... Args>
    void internal_inorder(Node* pNode, size_t level, Function func, Args const&... args) {
        if (pNode) {
            internal_inorder(pNode->getChild(0), level + 1, func, args...);
            func(pNode, level, args...);
            internal_inorder(pNode->getChild(1), level + 1, func, args...);
        }
    }

    // ========================================================================
    // POSTORDER VARIADIC CON LOCK
    // ========================================================================
    template <typename Function, typename... Args>
    void postorder_safe(Function func, Args const&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_postorder(m_pRoot, 0, func, args...);
    }
    
    template <typename Function, typename... Args>
    void internal_postorder(Node* pNode, size_t level, Function func, Args const&... args) {
        if (pNode) {
            internal_postorder(pNode->getChild(0), level + 1, func, args...);
            internal_postorder(pNode->getChild(1), level + 1, func, args...);
            func(pNode, level, args...);
        }
    }

    // TODO: Villanueva Richard
    // ========================================================================
    // PREORDER VARIADIC: Permite pasar múltiples funciones/argumentos
    // ========================================================================
    template <typename Function, typename... Args>
    void preorder(Function func, Args const&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_preorder(m_pRoot, 0, func, args...);
    }
    
    template <typename Function, typename... Args>
    void internal_preorder(Node* pNode, size_t level, Function func, Args const&... args) {
        if (pNode) {
            func(pNode, level, args...);
            internal_preorder(pNode->getChild(0), level + 1, func, args...);
            internal_preorder(pNode->getChild(1), level + 1, func, args...);
        }
    }

    // ========================================================================
    // PRINT: Reutiliza inorder variadic
    // ========================================================================
    void print(ostream &os) {
        inorder([](Node* pNode, size_t level, ostream& out) {
            Node *pParent = pNode->getParent();
            out << string(level * 3, ' ') << " | " << pNode->getDataRef()
                << "(" << (pParent ? to_string(pParent->getData()) : "Root") << ")" << endl;
        }, os);
    }
    
    // Versión original (mantenida para compatibilidad)
    void print_original(ostream &os)    {   print_original(m_pRoot, 0, os);  }
    // TODO: generalize this function by using iterators and apply any function
    void print_original(Node  *pNode, size_t level, ostream &os){
        if( pNode ){
            Node *pParent = pNode->getParent();
            print_original(pNode->getChild(1), level+1, os);
            os << string(" | ") * level << pNode->getDataRef() << "(" << (pParent?to_string(pParent->getData()):"Root") << ")" <<endl;
            print_original(pNode->getChild(0), level+1, os);
        }
    }

    // TODO: Open question for everyone
    // Generalizar el recorrido para recibir cualquier funcion
    // con una cantidad flexible de parametros conm variadic templates
    // https://en.cppreference.com/w/cpp/language/parameter_packs
    
    // TODO: Alcazar Joseph
    void postorder(Node  *pNode, size_t level, ostream &os){
        //foreach(postorderbegin(), postorderend(), fn)
        if( pNode ){   
            postorder(pNode->getChild(0), level+1, os);
            postorder(pNode->getChild(1), level+1, os);
            os << " --> " << pNode->getDataRef();
        }
    }
    
    // ========================================================================
    // WRITE Y OPERATOR <<
    // TODO: Arriola Aldo
    void Write(ostream &os) { 
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* pNode = m_pRoot;
        std::stack<Node*> stack;

        if (pNode) stack.push(pNode);
    
        while (!stack.empty()) {
            pNode = stack.top();
            stack.pop();
            os << pNode->getData() << " ";
            
            // Push derecho primero (para procesar izquierdo primero)
            if (pNode->getChild(1)) stack.push(pNode->getChild(1));
            if (pNode->getChild(0)) stack.push(pNode->getChild(0));
    }
    }

    // TODO: Toledo Oscar
    void Read(istream &is)  {
        std::lock_guard<std::mutex> lock(m_mutex);
        clear(m_pRoot);
        m_pRoot = nullptr;
        m_size = 0;
        value_type val; 
        while (is >> val) {
            LinkedValueType dummy = nullptr;
            insert(val, dummy);  
     }
};

// TODO: Arriola Aldo
// operator <<
template <typename Traits>
ostream & operator<<(ostream &os, CBinaryTree<Traits> &Tree){
    tree.print(os);
    return os;
}

// TODO: Toledo Oscar
template <typename Traits>
istream & operator>>(istream &is, CBinaryTree<Traits> &Tree){
    // Leer el arbol
    tree.print(is);
    return is;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__

