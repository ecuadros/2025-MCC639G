#ifndef __BTREE_H__
#define __BTREE_H__

#include "btreepage.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <stack>
#include <utility>

#define DEFAULT_BTREE_ORDER 3
const size_t MaxHeight = 5;

// ====================================================================
// TRAITS: BTreeTrait
// ====================================================================
template <typename _keyType, typename _ObjIDType> struct BTreeTrait {
  using keyType = _keyType;
  using ObjIDType = _ObjIDType;
};

// ====================================================================
// CLASE PRINCIPAL: BTree
// ====================================================================
template <typename Trait> class BTree {
public:
  using keyType = typename Trait::keyType;
  using ObjIDType = typename Trait::ObjIDType;
  using BTNode = CBTreePage<Trait>;
  using ObjectInfo = typename BTNode::ObjectInfo;
  using Container = BTree<Trait>;

  // ====================================================================
  // ITERADOR FORWARD: Recorrido inorder de izquierda a derecha
  // ====================================================================
  class forward_iterator {
  private:
    std::stack<std::pair<BTNode *, size_t>> m_stack;
    BTNode *m_pCurrent = nullptr;
    size_t m_currentIndex = 0;
    ObjectInfo *m_pCurrentInfo = nullptr;

    void pushLeftPath(BTNode *node) {
      while (node) {
        m_stack.push({node, 0});
        node = node->getSubPage(0);
      }
    }

  public:
    forward_iterator(BTNode *root = nullptr) {
      if (root) {
        pushLeftPath(root);
        advance();
      }
    }

    void advance() {
      if (m_stack.empty()) {
        m_pCurrentInfo = nullptr;
        return;
      }

      auto &top = m_stack.top();
      m_pCurrent = top.first;
      m_currentIndex = top.second;

      if (m_currentIndex < m_pCurrent->GetNumberOfKeys()) {
        m_pCurrentInfo = &m_pCurrent->getKey(m_currentIndex);
        top.second++;

        BTNode *rightChild = m_pCurrent->getSubPage(m_currentIndex + 1);
        if (rightChild) {
          pushLeftPath(rightChild);
        }
      } else {
        m_stack.pop();
        advance();
      }
    }

    forward_iterator &operator++() {
      advance();
      return *this;
    }

    ObjectInfo &operator*() { return *m_pCurrentInfo; }
    ObjectInfo *operator->() { return m_pCurrentInfo; }

    bool operator==(const forward_iterator &other) const {
      return m_pCurrentInfo == other.m_pCurrentInfo;
    }
    bool operator!=(const forward_iterator &other) const {
      return !(*this == other);
    }
  };

  // ====================================================================
  // ITERADOR BACKWARD: Recorrido inorder de derecha a izquierda
  // ====================================================================
  class backward_iterator {
  private:
    std::stack<std::pair<BTNode *, int>> m_stack;
    ObjectInfo *m_pCurrentInfo = nullptr;

    void pushRightPath(BTNode *node) {
      while (node) {
        int lastIdx = static_cast<int>(node->GetNumberOfKeys());
        m_stack.push({node, lastIdx});
        node = node->getSubPage(lastIdx);
      }
    }

  public:
    backward_iterator(BTNode *root = nullptr) {
      if (root) {
        pushRightPath(root);
        advance();
      }
    }

    void advance() {
      if (m_stack.empty()) {
        m_pCurrentInfo = nullptr;
        return;
      }

      auto &top = m_stack.top();
      BTNode *node = top.first;
      int idx = top.second;

      if (idx > 0) {
        idx--;
        top.second = idx;
        m_pCurrentInfo = &node->getKey(idx);

        BTNode *leftChild = node->getSubPage(idx);
        if (leftChild) {
          pushRightPath(leftChild);
        }
      } else {
        m_stack.pop();
        advance();
      }
    }

    backward_iterator &operator++() {
      advance();
      return *this;
    }

    ObjectInfo &operator*() { return *m_pCurrentInfo; }
    bool operator==(const backward_iterator &other) const {
      return m_pCurrentInfo == other.m_pCurrentInfo;
    }
    bool operator!=(const backward_iterator &other) const {
      return !(*this == other);
    }
  };

protected:
  BTNode m_Root;
  size_t m_Height;
  size_t m_Order;
  size_t m_NumKeys;
  bool m_Unique;

  // Concurrencia: mutex recursivo para permitir llamadas anidadas (Write ->
  // preorder)
  mutable std::recursive_mutex m_mutex;

public:
  BTree(size_t order = DEFAULT_BTREE_ORDER, bool unique = true)
      : m_Order(order), m_Root(2 * order + 1, unique), m_Unique(unique),
        m_NumKeys(0) {
    m_Root.SetMaxKeysForChilds(order);
    m_Height = 1;
  }

  BTree(BTree &&other) noexcept
      : m_Root(std::move(other.m_Root)),
        m_Height(std::exchange(other.m_Height, 0)), m_Order(other.m_Order),
        m_NumKeys(std::exchange(other.m_NumKeys, 0)), m_Unique(other.m_Unique) {
    std::scoped_lock lock(m_mutex, other.m_mutex);
  }

  ~BTree() {}

  bool Insert(const keyType key, const long ObjID);
  bool Remove(const keyType key, const long ObjID);

  ObjIDType Search(const keyType key) {
    std::scoped_lock lock(m_mutex);
    ObjIDType ObjID = -1;
    m_Root.Search(key, ObjID);
    return ObjID;
  }

  size_t size() const {
    std::scoped_lock lock(m_mutex);
    return m_NumKeys;
  }
  size_t height() const { return m_Height; }
  size_t GetOrder() const { return m_Order; }

  void Print(ostream &os) {
    std::scoped_lock lock(m_mutex);
    m_Root.Print(os);
  }

  template <typename Function, typename... Args>
  void inorder(Function func, Args &...args) {
    std::scoped_lock lock(m_mutex);
    m_Root.inorder(func, 0, args...);
  }

  template <typename Function, typename... Args>
  void preorder(Function func, Args &...args) {
    std::scoped_lock lock(m_mutex);
    m_Root.preorder(func, 0, args...);
  }

  template <typename Function, typename... Args>
  void postorder(Function func, Args &...args) {
    std::scoped_lock lock(m_mutex);
    m_Root.postorder(func, 0, args...);
  }

  template <typename Predicate, typename... Args>
  ObjectInfo *firstThat(Predicate pred, Args &...args) {
    std::scoped_lock lock(m_mutex);
    return m_Root.firstThat(pred, 0, args...);
  }

  forward_iterator begin() {
    std::scoped_lock lock(m_mutex);
    return forward_iterator(&m_Root);
  }
  forward_iterator end() { return forward_iterator(nullptr); }

  backward_iterator rbegin() {
    std::scoped_lock lock(m_mutex);
    return backward_iterator(&m_Root);
  }
  backward_iterator rend() { return backward_iterator(nullptr); }

  void Write(ostream &os) {
    std::scoped_lock lock(m_mutex);
    preorder([](ObjectInfo &info, size_t level,
                ostream &out) { out << info.key << " " << info.ObjID << " "; },
             os);
  }

  void Read(istream &is) {
    std::scoped_lock lock(m_mutex);
    keyType key;
    ObjIDType objId;
    while (is >> key >> objId) {
      Insert(key, objId); // Insert maneja incremento de m_NumKeys
    }
  }

  // Compatibilidad
  typedef typename BTNode::lpfnForEach2 lpfnForEach2;
  typedef typename BTNode::lpfnForEach3 lpfnForEach3;
  typedef typename BTNode::lpfnFirstThat2 lpfnFirstThat2;
  typedef typename BTNode::lpfnFirstThat3 lpfnFirstThat3;

  void ForEach(lpfnForEach2 lpfn, void *pExtra1) {
    m_Root.ForEach(lpfn, 0, pExtra1);
  }
  void ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2) {
    m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
  }
  ObjectInfo *FirstThat(lpfnFirstThat2 lpfn, void *pExtra1) {
    return m_Root.FirstThat(lpfn, 0, pExtra1);
  }
  ObjectInfo *FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2) {
    return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
  }
};

// Implementaciones externas de métodos template
template <typename Trait>
bool BTree<Trait>::Insert(const keyType key, const long ObjID) {
  std::scoped_lock lock(m_mutex);
  bt_ErrorCode error = m_Root.Insert(key, ObjID);
  if (error == bt_duplicate)
    return false;
  m_NumKeys++;
  if (error == bt_overflow) {
    m_Root.SplitRoot();
    m_Height++;
  }
  return true;
}

template <typename Trait>
bool BTree<Trait>::Remove(const keyType key, const long ObjID) {
  std::scoped_lock lock(m_mutex);
  bt_ErrorCode error = m_Root.Remove(key, ObjID);
  if (error == bt_duplicate || error == bt_nofound)
    return false;
  m_NumKeys--;
  if (error == bt_rootmerged)
    m_Height--;
  return true;
}

template <typename Trait> ostream &operator<<(ostream &os, BTree<Trait> &tree) {
  os << "BTree order=" << tree.GetOrder() << " size=" << tree.size()
     << " height=" << tree.height() << endl;
  tree.Print(os);
  return os;
}

template <typename Trait> istream &operator>>(istream &is, BTree<Trait> &tree) {
  tree.Read(is);
  return is;
}

void DemoBTree();

#endif // __BTREE_H__