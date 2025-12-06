#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"
#include <map>

template <typename _T> struct AVLAscTraits {
  using T = _T;
  using Node = CBinaryTreeNode<AVLAscTraits<_T>>;
  using CompareFn = less<T>;
};

template <typename _T> struct AVLDescTraits {
  using T = _T;
  using Node = CBinaryTreeNode<AVLDescTraits<_T>>;
  using CompareFn = greater<T>;
};

// ====================================================================
// CLASE CAVLTree: Hereda de CBinaryTree con balanceo automático
// Usa un mapa para almacenar alturas de nodos
// ====================================================================
template <typename Traits> class CAVLTree : public CBinaryTree<Traits> {
public:
  using Base = CBinaryTree<Traits>;
  using Node = typename Traits::Node;
  using value_type = typename Traits::T;
  using CompareFn = typename Traits::CompareFn;
  using Container = CAVLTree<Traits>;

  // Constructor por defecto
  CAVLTree() : Base() {}
  // Heredamos iteradores de CBinaryTree - compatible con foreach
  using forward_iterator = binary_tree_iterator<Container, true>;
  using backward_iterator = binary_tree_iterator<Container, false>;

protected:
  // Factor de balance máximo permitido (1 para AVL estándar)
  int m_balanceFactor = 1;

private:
  // Mapa de alturas usando dirección del nodo como clave
  std::map<Node *, int> m_heights;
  CompareFn Compfn;

  // Obtener altura de un nodo (1 si es hoja, 0 si es nullptr)
  int getHeight(Node *pNode) {
    if (!pNode)
      return 0;
    auto it = m_heights.find(pNode);
    return (it != m_heights.end()) ? it->second : 1;
  }

  // Establecer altura de un nodo
  void setHeight(Node *pNode, int h) {
    if (pNode)
      m_heights[pNode] = h;
  }

  // Actualizar altura de un nodo basado en sus hijos
  void updateHeight(Node *pNode) {
    if (pNode) {
      int leftH = getHeight(static_cast<Node *>(pNode->getChild(0)));
      int rightH = getHeight(static_cast<Node *>(pNode->getChild(1)));
      setHeight(pNode, 1 + max(leftH, rightH));
    }
  }

  // Obtener factor de balance
  int getBalance(Node *pNode) {
    if (!pNode)
      return 0;
    return getHeight(static_cast<Node *>(pNode->getChild(0))) -
           getHeight(static_cast<Node *>(pNode->getChild(1)));
  }

  // Rotación simple a la derecha (LL)
  Node *rotateRight(Node *y) {
    Node *x = static_cast<Node *>(y->getChild(0));
    Node *T2 = static_cast<Node *>(x->getChild(1));

    // Realizar rotación
    x->setChild(y, 1);
    y->setChild(T2, 0);

    // Actualizar alturas
    updateHeight(y);
    updateHeight(x);

    return x;
  }

  // Rotación simple a la izquierda (RR)
  Node *rotateLeft(Node *x) {
    Node *y = static_cast<Node *>(x->getChild(1));
    Node *T2 = static_cast<Node *>(y->getChild(0));

    // Realizar rotación
    y->setChild(x, 0);
    x->setChild(T2, 1);

    // Actualizar alturas
    updateHeight(x);
    updateHeight(y);

    return y;
  }

  // Crear nodo
  Node *CreateNode(Node *pParent, value_type elem, Ref ref) {
    Node *newNode = new Node(pParent, elem, ref);
    setHeight(newNode, 1);
    return newNode;
  }

  // Inserción con balance AVL
  virtual Node *internal_insert(value_type elem, Ref ref, Node *pParent,
                                Node *&rpOrigin) {
    // 1. Inserción BST normal
    if (!rpOrigin) {
      ++Base::m_size;
      rpOrigin = CreateNode(pParent, elem, ref);
      return rpOrigin;
    }

    // Decidir rama (0 = izquierda, 1 = derecha)
    if (Compfn(elem, rpOrigin->getDataRef())) {
      Node *&leftChild = reinterpret_cast<Node *&>(rpOrigin->getChildRef(0));
      internal_insert(elem, ref, rpOrigin, leftChild);
    } else {
      Node *&rightChild = reinterpret_cast<Node *&>(rpOrigin->getChildRef(1));
      internal_insert(elem, ref, rpOrigin, rightChild);
    }

    // 2. Actualizar altura del nodo actual
    updateHeight(rpOrigin);

    // 3. Obtener factor de balance
    int balance = getBalance(rpOrigin);

    // 4. Casos de desbalance y rotaciones

    // Caso LL: Desbalance izquierda-izquierda
    if (balance > m_balanceFactor && rpOrigin->getChild(0) &&
        Compfn(elem,
               static_cast<Node *>(rpOrigin->getChild(0))->getDataRef())) {
      rpOrigin = rotateRight(rpOrigin);
      return rpOrigin;
    }

    // Caso RR: Desbalance derecha-derecha
    if (balance < -m_balanceFactor && rpOrigin->getChild(1) &&
        !Compfn(elem,
                static_cast<Node *>(rpOrigin->getChild(1))->getDataRef())) {
      rpOrigin = rotateLeft(rpOrigin);
      return rpOrigin;
    }

    // Caso LR: Desbalance izquierda-derecha
    if (balance > m_balanceFactor && rpOrigin->getChild(0) &&
        !Compfn(elem,
                static_cast<Node *>(rpOrigin->getChild(0))->getDataRef())) {
      Node *rotated = rotateLeft(static_cast<Node *>(rpOrigin->getChild(0)));
      rpOrigin->setChild(rotated, 0);
      rpOrigin = rotateRight(rpOrigin);
      return rpOrigin;
    }

    // Caso RL: Desbalance derecha-izquierda
    if (balance < -m_balanceFactor && rpOrigin->getChild(1) &&
        Compfn(elem,
               static_cast<Node *>(rpOrigin->getChild(1))->getDataRef())) {
      Node *rotated = rotateRight(static_cast<Node *>(rpOrigin->getChild(1)));
      rpOrigin->setChild(rotated, 1);
      rpOrigin = rotateLeft(rpOrigin);
      return rpOrigin;
    }

    return rpOrigin;
  }

public:
  // Destructor - limpia el mapa de alturas
  virtual ~CAVLTree() { m_heights.clear(); }

  // Inserción pública - sobrescribe para usar nuestra versión con balance
  void insert(value_type elem, Ref ref) override {
    Base::m_pRoot = internal_insert(elem, ref, nullptr, Base::m_pRoot);
  }

  // Heredamos de CBinaryTree:
  // - inorder, preorder, postorder (variadic)
  // - inorder_print
  // - Write, Read
  // - operator<<, operator>>
  // - begin(), end(), rbegin(), rend() para foreach
};

// Declaración de la función demo
void DemoAVLTree();

#endif // __AVL_H__