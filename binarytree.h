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
        : m_pParent(pParent), m_data(data), m_ref(ref), m_pChild(2, nullptr)
    {
        m_pChild[0] = p0;
        m_pChild[1] = p1;
    }

    // TODO: Keynode 
    value_type         getData() const       {   return m_data;    }
    Ref &              getRef()              {   return m_ref;    }
 
 // TODO: review if these functions must remain public/private
    void   setChild(Node *pChild, size_t pos)  {   if (pos < 2) m_pChild[pos] = pChild;  }
    Node * getChild(size_t branch) { return (branch < 2) ? m_pChild[branch] : nullptr; }
    Node *& getChildRef(size_t branch) { return m_pChild[branch]; }
    Node * getParent() { return m_pParent; }
    size_t getChildCount() const { return m_pChild.size(); }
};

// revisado  
// ====================================================================
// ITERADORES
// ====================================================================
template <typename Container, bool IsForward = true>
class binary_tree_iterator : public general_iterator<Container,  class binary_tree_iterator<Container> > // 
{
public:
  using Parent    = class general_iterator<Container, binary_tree_iterator<Container> >;
  using Node      = typename Container::Node;
  using Container = binary_tree_iterator<Container>;
  using value_type = typename Container::value_type;
  using iterator = binary_tree_iterator<Container, IsForward>;

  public:
    binary_tree_iterator(Container *pContainer, Node *pNode) : Parent (pContainer,pNode) {}
    binary_tree_iterator(Container &other)  : Parent (other) {}
    binary_tree_iterator(Container &&other) : Parent(other) {} // Move constructor C++11 en adelante

private:
  std::stack<Node*> m_stack;
  Node *m_pCurrent = nullptr;

// Empuja nodos hacia la izquierda (forward) o derecha (backward)
  void pushToLeaf(Node* node) {
      while (node) {
        m_stack.push(node);
        if constexpr (IsForward)
          node = node->getChild(0);   // Izquierda para forward
        else
          node = node->getChild(1);   // Derecha para backward
      }
  }

public:
    // TODO: Fuentes Patrick
    binary_tree_iterator operator++() {
        Parent::m_pNode = Parent::m_pNode ? (Node*)Parent::m_pNode->getpNext() : nullptr;
      }

      binary_tree_iterator(Nodo *root = nullptr) {
        if (root) pushToLeaf(root);
        advance(); // Posiciona el primer elemento
      }

      // Avanza al siguiente nodo en el recorrido
      void advance() {
          if (m_stack.empty()) {
              m_pCurrent = nullptr;
              return;
          }

          m_pCurrent = m_stack.top();
          m_stack.pop();

          // Procede al siguiente nodo
          if constexpr (IsForward) {
              if (m_pCurrent->getChild(1))
                pushToLeaf(m_pCurrent->getChild(1)); // Hijo derecho
          } else {
              if (m_pCurrent->getChild(0))
                pushToLeaf(m_pCurrent->getChild(0)); // Hijo izquierdo
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

// ====================================================================
// TRAITS: Ascendente y Descendente
// ====================================================================
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

// ====================================================================
// CLASE PRINCIPAL CBinaryTree
// ====================================================================
template <typename Traits>
class CBinaryTree{
  public:
    using value_type    = typename Traits::T;
    using Node          = typename Traits::Node;
    
    using CompareFn     = typename Traits::CompareFn;
    using Container     = CBinaryTree<Traits>;
    using iterator      = binary_tree_iterator<Container>;

    // ITERADORES: forward (izq→der) y backward (der→izq)
    using forward_iterator  = binary_tree_iterator<Container, true>;
    using backward_iterator = binary_tree_iterator<Container, false>;

protected:
    Node    *m_pRoot = nullptr;
    size_t   m_size  = 0;
    CompareFn Compfn;

    // Concurrency mutex
    mutable std::mutex m_mutex;

public: 
    size_t  size()  const       { return m_size;       }
    bool    empty() const       { return m_size == 0;  }

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
            rpOrigin = CreateNode(pParent, elem, ref);
            return rpOrigin;
        }

        size_t branch = Compfn(elem, rpOrigin->getDataRef()) ? 0 : 1;
        return internal_insert(elem, ref, nullptr, rpOrigin, rpOrigin->getChildRef(branch));
    }


// ====================================================================
// CONSTRUCTOR COPIA: Transfiere la propiedad de los nodos
// ==================================================================== 
CBinaryTree(myself &&other): m_pRoot(other.m_pRoot), m_size(other.m_size), Compfn(other.Compfn){

    std:scope_lock lock(m_mutex, other.m_mutex);
    m_pRoot = std::exchange(other.m_pRoot, nullptr);
    m_size = std::exchange(other.m_size, 0);
    Compfn = std::move(other.Compfn, nullptr);
}


 // ====================================================================
// CONSTRUCTOR MOVE: Transfiere la propiedad de los nodos
// ==================================================================== 
CBinaryTree(myself &&other) {
    std::scope_lock lock(m_mutex, other.m_mutex);
    m_pRoot = std::exchange(other.m_pRoot, nullptr); 
    m_size = std::exchange(other.m_size, 0);
 }
}



// ====================================================================
// DESCTRUCTOR + LIMPIEZA
// ==================================================================== 
public:
  // TODO: Selis Luis (Destructor)
  virtual ~CBinaryTree(){
      std::lock_guard<std::mutex> lock(m_mutex);
      clear();
      m_pRoot = nullptr;
      m_size = 0;
  }


  // TODO: Quispe David
  void inorder (ostream &os) { inorder (m_pRoot, os, 0); }

  // TODO: Quispe David
  void inorder(Node *pNode, ostream &os, size_t level){
      if (pNode) {
          //Node *pParent = pNode->getParent();
          inorder(pNode->getChild(0), os, level+1);
          os << " " << pNode->getDataRef();
          inorder(pNode->getChild(1), os, level+1);
      }
  }

  void clear(Node *pNode){
      std::lock_guard<std::mutex> lock(m_mutex);
      clearTree(m_pRoot);
      m_pRoot = nullptr;
      m_size = 0;
  }

  size_t size(){
      std::lock_guard<std::mutex> lock(m_mutex);
      return m_size;
  }
  
// ====================================================================
// ITERADORES: begin/end
// ====================================================================  

  forward_iterator begin(){
      std::lock_guard<std::mutex> lock(m_mutex);
      return forward_iterator(m_pRoot);
  }

  forward_iterator end() { return forward_iterator(nullptr); }

  backward_iterator rbegin(){
      std::lock_guard<std::mutex> lock(m_mutex);
      return backward_iterator(m_pRoot);
  }

  backward_iterator rend() { return backward_iterator(nullptr); }

// ====================================================================
// VARIADIC inorder
// ====================================================================

  void inorder(Node *pNode, void (*visit) (value_type& item)){
      if (pNode){
          inorder(pNode->getChild(0), *visit);
          (*visit)( pNode->getDataRef() );
          inorder(pNode->getChild(1), *visit);
      }
  }

  template <typename Function, typename... Args>
  void inorder(Function func, Args const&... args){
      std::lock_guard<std::mutex> lock(m_mutex);
      internal_inorder(m_pRoot, 0, func, args...);  //:forward
  }

  template <typename Function, typename... Args>
  void internal_inorder(Node* pNode, size_t level, Function func, Args const&... args){
      if (pNode) {
          internal_inorder(pNode->getChild(0), level + 1, func, args...);
          func(pNode, level, args...);
          internal_inorder(pNode->getChild(1), level + 1, func, args...);
      }
  }

// ====================================================================
// VARIADIC postorder
// ====================================================================

  template <typename Function, typename... Args>
  void postorder(Function func, Args const&... args){
      std::lock_guard<std::mutex> lock(m_mutex);
      internal_postorder(m_pRoot, 0, func, args...);
  }

  template <typename Function, typename... Args>
  void postorder(Node *pNode, size_t level, Function func, Args const&... args){
      if (pNode) {
          postorder(pNode->getChild(0), level + 1, func, args...);
          postorder(pNode->getChild(1), level + 1, func, args...);
          func(pNode, level);
      }
  }

  template <typename Function, typename... Args>
  void internal_postorder(Node *pNode, size_t level, Function func, Args const&... args){
      if (pNode) {
          internal_postorder(pNode->getChild(0), level + 1, func, args...);
          internal_postorder(pNode->getChild(1), level + 1, func, args...);
          func(pNode, level, args...);
      }
  }

// ====================================================================
// VARIADIC preorder
// ====================================================================

  void preorder (ostream &os) { preorder (m_pRoot, os, 0); }

  void preorder(Node *pNode, size_t level, ostream &os){
      if (pNode) {
          os << " " << pNode->getDataRef();
          preorder(pNode->getChild(0), level+1, os);
          preorder(pNode->getChild(1), level+1, os);
      }
  }

  template <typename Function, typename... Args>
  void preorder(Function func, Args const&... args){
      std::lock_guard<std::mutex> lock(m_mutex);
      internal_preorder(m_pRoot, 0, func, args...);
  }

  template <typename Function, typename... Args>
  void internal_preorder(Node *pNode, size_t level, Function func, Args const&... args){
      if (pNode) {
          func(pNode, level, args...);
          internal_preorder(pNode->getChild(0), level + 1, func, args...);
          internal_preorder(pNode->getChild(1), level + 1, func, args...);
      }
  }

// ====================================================================
// PRINT
// ====================================================================
  // PRINT INORDER
  void print(ostream &os) {
      inorder([&](Node* pNode, size_t level, ostream& out) {
          Node *pParent = pNode->getParent(); 
          out << string(level * 3, ' ') << "| " << pNode->getDataRef()
              << " (" << (pParent ? to_string(pParent->getData()) : "Root") << ")" << endl;
      }, os);
  }

// ====================================================================
// PRINT ORIGINAL
// ====================================================================

  void print_original(ostream &os) { print_original(m_pRoot, 0, os); }

  void print_original(Node *pNode, size_t level, ostream &os){
      if (pNode) {
          Node *pParent = pNode->getParent();
          print(pNode->getChild(0), level+1, os);
          os << string(" ") * level << pNode->getDataRef() << "(" << (pParent?to_string(pParent->getData()):"Root") << ")" << endl;
          print(pNode->getChild(0), level+1, os);
          print_original(pNode->getChild(1), level+1, os);
      }
  }

// ====================================================================
// POSTORDER (no variadic)
// ====================================================================

  void postorder(Node *pNode, size_t level, ostream &os){
      if (pNode) {
          postorder(pNode->getChild(0), level+1, os);
          postorder(pNode->getChild(1), level+1, os);
          os << " " << pNode->getDataRef();
      }
  }

// ====================================================================
// WRITE
// ====================================================================
    // TODO: Arriola Aldo
    void Write(ostream &os) { os << *this;  }

    // TODO: Toledo Oscar    
    void Write(ostream &os) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* pNode = m_pRoot;
        std::stack<Node*> stack;

        if (pNode) stack.push(pNode);

        while (!stack.empty()) {
            pNode = stack.top();
            stack.pop();
            os << pNode->getData() << " ";

            // Push derecho primero para que el izquierdo se procese primero
            if (pNode->getChild(1)) stack.push(pNode->getChild(1));
            if (pNode->getChild(0)) stack.push(pNode->getChild(0));
        }
    }

// ====================================================================
// READ
// ====================================================================

  void Read(istream &is) { /* TODO */ }

  void Read(istream &is){
      std::lock_guard<std::mutex> lock(m_mutex);
      clear(m_pRoot);
      m_pRoot = nullptr;
      m_size = 0;
      value_type val;
      while (is >> val) {
          LinkedValueType dummy = nullptr;
          insert(val, dummy);
      }
  }


// ====================================================================
// OPERADORES << Y >>
// ====================================================================

// TODO: Arriola Aldo
// operator <<
template <typename Traits>
ostream &operator<<(ostream &os, CBinaryTree<Traits> &obj){
    os << "CBinaryTree with " << obj.size() << " elements.";
    obj.inorder(os);
    return os;
}

// TODO: Toledo Oscar
template <typename Traits>
istream &operator>>(istream &is, CBinaryTree<Traits> &tree){
    tree.print(is);
    return is;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__ 