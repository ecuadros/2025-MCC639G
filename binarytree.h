#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include "types.h"
#include <cassert>
#include <functional>
#include <iostream>
#include <mutex>
#include <stack>
#include <string>
#include <vector>
using namespace std;

template <typename Traits> class CBinaryTreeNode {
public:
  using value_type = typename Traits::T;
  using Node = CBinaryTreeNode<Traits>;

private:
  value_type m_data;
  Node *m_pParent = nullptr;
  Ref m_ref;
  vector<Node *> m_pChild = {nullptr,
                             nullptr}; // 2 hijos inicializados en nullptr

public:
  CBinaryTreeNode(Node *pParent, value_type data, Ref ref, Node *p0 = nullptr,
                  Node *p1 = nullptr)
      : m_data(data), m_pParent(pParent), m_ref(ref), m_pChild(2, nullptr) {
    m_pChild[0] = p0;
    m_pChild[1] = p1;
  }

  value_type getData() const { return m_data; }
  value_type &getDataRef() { return m_data; }
  Ref &getRef() { return m_ref; }

  // TODO: review if these functions must remain public/private
  void setChild(Node *pChild, size_t pos) {
    if (pos < 2)
      m_pChild[pos] = pChild;
  }
  Node *getChild(size_t branch) {
    return (branch < 2) ? m_pChild[branch] : nullptr;
  }
  Node *&getChildRef(size_t branch) { return m_pChild[branch]; }
  Node *getParent() { return m_pParent; }
  size_t getChildCount() const { return m_pChild.size(); }
};

// ====================================================================
// ITERADORES
// ====================================================================
template <typename ContainerType, bool IsForward = true>
class binary_tree_iterator {
public:
  using Node = typename ContainerType::Node;
  using Container = ContainerType;
  using value_type = typename ContainerType::value_type;
  using iterator = binary_tree_iterator<ContainerType, IsForward>;

private:
  std::stack<Node *> m_stack;
  Node *m_pCurrent = nullptr;

  // Empuja nodos hacia la izquierda (forward) o derecha (backward)
  void pushToLeaf(Node *node) {
    while (node) {
      m_stack.push(node);
      if constexpr (IsForward)
        node = node->getChild(0); // Izquierda para forward
      else
        node = node->getChild(1); // Derecha para backward
    }
  }

public:
  binary_tree_iterator(Node *root = nullptr) : m_pCurrent(nullptr) {
    if (root)
      pushToLeaf(root);
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

  iterator &operator++() {
    advance();
    return *this;
  }

  value_type &operator*() { return m_pCurrent->getDataRef(); }

  bool operator==(const iterator &other) const {
    return m_pCurrent == other.m_pCurrent;
  }

  bool operator!=(const iterator &other) const { return !(*this == other); }
};

// ====================================================================
// TRAITS: Ascendente y Descendente
// ====================================================================
template <typename _T> struct BinaryTreeAscTraits {
  using T = _T;
  using Node = CBinaryTreeNode<BinaryTreeAscTraits<_T>>;
  using CompareFn = less<T>;
};

template <typename _T> struct BinaryTreeDescTraits {
  using T = _T;
  using Node = CBinaryTreeNode<BinaryTreeDescTraits<_T>>;
  using CompareFn = greater<T>;
};

// ====================================================================
// CLASE PRINCIPAL CBinaryTree
// ====================================================================
template <typename Traits> class CBinaryTree {
public:
  using value_type = typename Traits::T;
  using Node = typename Traits::Node;

  using CompareFn = typename Traits::CompareFn;
  using Container = CBinaryTree<Traits>;
  using iterator = binary_tree_iterator<Container>;

  // ITERADORES: forward (izq→der) y backward (der→izq)
  using forward_iterator = binary_tree_iterator<Container, true>;
  using backward_iterator = binary_tree_iterator<Container, false>;

protected:
  Node *m_pRoot = nullptr;
  size_t m_size = 0;
  CompareFn Compfn;

  // Concurrency mutex
  mutable std::mutex m_mutex;

public:
  size_t size() const { return m_size; }
  bool empty() const { return m_size == 0; }

  // TODO: insert must receive two paramaters: elem and LinkedValueType value
  virtual void insert(value_type elem, Ref ref) {
    m_pRoot = internal_insert(elem, ref, nullptr, m_pRoot);
  }

protected:
  Node *CreateNode(Node *pParent, value_type elem, Ref ref) {
    return new Node(pParent, elem, ref);
  }
  Node *internal_insert(value_type elem, Ref ref, Node *pParent,
                        Node *&rpOrigin) {
    if (!rpOrigin) {
      ++m_size;
      rpOrigin = CreateNode(pParent, elem, ref);
      return rpOrigin;
    }

    size_t branch = Compfn(elem, rpOrigin->getDataRef()) ? 0 : 1;
    return internal_insert(elem, ref, rpOrigin, rpOrigin->getChildRef(branch));
  }

  Node *copyNodes(Node *pNode, Node *pParent) {
    if (!pNode)
      return nullptr;
    Node *newNode = CreateNode(pParent, pNode->getData(), pNode->getRef());
    newNode->setChild(copyNodes(pNode->getChild(0), newNode), 0);
    newNode->setChild(copyNodes(pNode->getChild(1), newNode), 1);
    return newNode;
  }

public:
  // Constructor por defecto
  CBinaryTree() : m_pRoot(nullptr), m_size(0) {}

  // ====================================================================
  // CONSTRUCTOR COPIA: Copia profunda de los nodos
  // ====================================================================
  CBinaryTree(const CBinaryTree &other) {
    std::scoped_lock lock(m_mutex, other.m_mutex);
    m_size = other.m_size;
    m_pRoot = copyNodes(other.m_pRoot, nullptr);
  }

  // ====================================================================
  // CONSTRUCTOR MOVE: Transfiere la propiedad de los nodos
  // ====================================================================
  CBinaryTree(CBinaryTree &&other) noexcept {
    std::scoped_lock lock(m_mutex, other.m_mutex);
    m_pRoot = std::exchange(other.m_pRoot, nullptr);
    m_size = std::exchange(other.m_size, 0);
    Compfn = std::move(other.Compfn);
  }

  // ====================================================================
  // DESCTRUCTOR + LIMPIEZA
  // ====================================================================
public:
  // TODO: Selis Luis (Destructor)
  virtual ~CBinaryTree() {
    std::scoped_lock lock(m_mutex);
    clear();
    m_pRoot = nullptr;
    m_size = 0;
  }

  // TODO: Quispe David
  void inorder_print(ostream &os) { inorder_print(m_pRoot, os, 0); }

  // TODO: Quispe David
  void inorder_print(Node *pNode, ostream &os, size_t level) {
    if (pNode) {
      inorder_print(pNode->getChild(0), os, level + 1);
      os << " " << pNode->getData();
      inorder_print(pNode->getChild(1), os, level + 1);
    }
  }

  void clear() {
    clearTree(m_pRoot);
    m_pRoot = nullptr;
    m_size = 0;
  }

  void clearTree(Node *pNode) {
    if (pNode) {
      clearTree(pNode->getChild(0));
      clearTree(pNode->getChild(1));
      delete pNode;
    }
  }

  // ====================================================================
  // ITERADORES: begin/end
  // ====================================================================

  forward_iterator begin() {
    std::scoped_lock lock(m_mutex);
    return forward_iterator(m_pRoot);
  }

  forward_iterator end() { return forward_iterator(nullptr); }

  backward_iterator rbegin() {
    std::scoped_lock lock(m_mutex);
    return backward_iterator(m_pRoot);
  }

  backward_iterator rend() { return backward_iterator(nullptr); }

  // ====================================================================
  // VARIADIC inorder
  // ====================================================================

  void inorder(Node *pNode, void (*visit)(value_type &item)) {
    if (pNode) {
      inorder(pNode->getChild(0), *visit);
      (*visit)(pNode->getDataRef());
      inorder(pNode->getChild(1), *visit);
    }
  }

  template <typename Function, typename... Args>
  void inorder(Function func, Args const &...args) {
    std::scoped_lock lock(m_mutex);
    internal_inorder(m_pRoot, 0, func, args...); //: forward
  }

  template <typename Function, typename... Args>
  void internal_inorder(Node *pNode, size_t level, Function func,
                        Args const &...args) {
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
  void postorder(Function func, Args const &...args) {
    std::scoped_lock lock(m_mutex);
    internal_postorder(m_pRoot, 0, func, args...);
  }

  template <typename Function, typename... Args>
  void postorder(Node *pNode, size_t level, Function func,
                 Args const &...args) {
    if (pNode) {
      postorder(pNode->getChild(0), level + 1, func, args...);
      postorder(pNode->getChild(1), level + 1, func, args...);
      func(pNode, level);
    }
  }

  template <typename Function, typename... Args>
  void internal_postorder(Node *pNode, size_t level, Function func,
                          Args const &...args) {
    if (pNode) {
      internal_postorder(pNode->getChild(0), level + 1, func, args...);
      internal_postorder(pNode->getChild(1), level + 1, func, args...);
      func(pNode, level, args...);
    }
  }

  // ====================================================================
  // VARIADIC preorder
  // ====================================================================

  void preorder(ostream &os) { preorder(m_pRoot, os, 0); }

  void preorder(Node *pNode, size_t level, ostream &os) {
    if (pNode) {
      os << " " << pNode->getData();
      preorder(pNode->getChild(0), level + 1, os);
      preorder(pNode->getChild(1), level + 1, os);
    }
  }

  template <typename Function, typename... Args>
  void preorder(Function func, Args const &...args) {
    std::scoped_lock lock(m_mutex);
    internal_preorder(m_pRoot, 0, func, args...);
  }

  template <typename Function, typename... Args>
  void internal_preorder(Node *pNode, size_t level, Function func,
                         Args const &...args) {
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
    inorder(
        [&](Node *pNode, size_t level, ostream &out) {
          Node *pParent = pNode->getParent();
          out << string(level * 3, ' ') << "| " << pNode->getDataRef() << " ("
              << (pParent ? to_string(pParent->getData()) : "Root") << ")"
              << endl;
        },
        os);
  }

  // ====================================================================
  // PRINT ORIGINAL
  // ====================================================================

  void print_original(ostream &os) { print_original(m_pRoot, 0, os); }

  void print_original(Node *pNode, size_t level, ostream &os) {
    if (pNode) {
      Node *pParent = pNode->getParent();
      print_original(pNode->getChild(0), level + 1, os);
      os << string(level, ' ') << pNode->getData() << "("
         << (pParent ? to_string(pParent->getData()) : "Root") << ")" << endl;
      print_original(pNode->getChild(1), level + 1, os);
    }
  }

  // ====================================================================
  // POSTORDER (no variadic)
  // ====================================================================

  void postorder(Node *pNode, size_t level, ostream &os) {
    if (pNode) {
      postorder(pNode->getChild(0), level + 1, os);
      postorder(pNode->getChild(1), level + 1, os);
      os << " " << pNode->getData();
    }
  }

  // ====================================================================
  // WRITE
  // ====================================================================
  // TODO: Arriola Aldo / Toledo Oscar
  void Write(ostream &os) {
    std::scoped_lock lock(m_mutex);
    Node *pNode = m_pRoot;
    std::stack<Node *> stack;

    if (pNode)
      stack.push(pNode);

    while (!stack.empty()) {
      pNode = stack.top();
      stack.pop();
      os << pNode->getData() << " ";

      // Push derecho primero para que el izquierdo se procese primero
      if (pNode->getChild(1))
        stack.push(pNode->getChild(1));
      if (pNode->getChild(0))
        stack.push(pNode->getChild(0));
    }
  }

  // ====================================================================
  // READ
  // ====================================================================

  void Read(istream &is) {
    std::scoped_lock lock(m_mutex);
    clear();
    value_type val;
    Ref dummy{};
    while (is >> val) {
      insert(val, dummy);
    }
  }

}; // Fin de la clase CBinaryTree

// ====================================================================
// OPERADORES << Y >> (funciones libres, fuera de la clase)
// ====================================================================

// TODO: Arriola Aldo
// operator <<
template <typename Traits>
ostream &operator<<(ostream &os, CBinaryTree<Traits> &obj) {
  os << "CBinaryTree with " << obj.size() << " elements:";
  obj.inorder_print(os);
  return os;
}

// TODO: Toledo Oscar
template <typename Traits>
istream &operator>>(istream &is, CBinaryTree<Traits> &tree) {
  tree.Read(is);
  return is;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__