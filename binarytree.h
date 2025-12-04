#ifndef __BINARY_TREE_H__  
#define __BINARY_TREE_H__ 
#include <cassert>
#include <functional>
#include <stack>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include "types.h"

using namespace std;

template <typename Container, typename IteratorType>
class general_iterator {
protected:
    Container* m_pContainer;
    typename Container::Node* m_pNode;

public:
    general_iterator(Container* pContainer, typename Container::Node* pNode) 
        : m_pContainer(pContainer), m_pNode(pNode) {}

    general_iterator(const general_iterator& other) 
        : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode) {}
        
    general_iterator(general_iterator&& other) 
        : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode) {
        other.m_pContainer = nullptr;
        other.m_pNode = nullptr;
    }

    // basic operators
    bool operator==(const IteratorType& other) const { return m_pNode == other.m_pNode; }
    bool operator!=(const IteratorType& other) const { return m_pNode != other.m_pNode; }
    
    typename Container::value_type& operator*() { return m_pNode->getDataRef(); }
    typename Container::value_type* operator->() { return &(m_pNode->getDataRef()); }
};

template <typename Traits>
class CBinaryTreeNode {
public:
    typedef typename Traits::T value_type;
    
private:
    typedef CBinaryTreeNode<Traits> Node;
    
public:
    value_type m_data;
    Node* m_pParent;
    Ref m_ref;
    vector<Node*> m_pChild;

public:
    CBinaryTreeNode(Node* pParent, value_type data, Ref ref, 
                   Node* p0 = nullptr, Node* p1 = nullptr) 
        : m_data(data), m_pParent(pParent), m_ref(ref), m_pChild(2, nullptr) {
        m_pChild[0] = p0;
        m_pChild[1] = p1;
    }

    value_type getData() const { return m_data; }
    Ref& getDataRef() { return m_ref; }
    
    void setpChild(Node* pChild, size_t pos) { 
        if (pos < 2) m_pChild[pos] = pChild; 
    }
    
    Node* getChild(size_t branch) const { 
        return (branch < 2) ? m_pChild[branch] : nullptr; 
    }
    
    Node*& getChildRef(size_t branch) { 
        return m_pChild[branch]; 
    }
    
    Node* getParent() const { return m_pParent; }
    
    size_t getBranch() const {
        if (!m_pParent) return 0;
        return (m_pParent->m_pChild[1] == this) ? 1 : 0;
    }
};

template <typename Container>
class binary_tree_forward_iterator : public general_iterator<Container, binary_tree_forward_iterator<Container>> {
public:
    typedef typename Container::Node Node;
    typedef binary_tree_forward_iterator<Container> myself;
    typedef general_iterator<Container, myself> Parent;

    binary_tree_forward_iterator(Container* pContainer, Node* pNode) : Parent(pContainer, pNode) {}
    binary_tree_forward_iterator(const myself& other) : Parent(other) {}
    binary_tree_forward_iterator(myself&& other) : Parent(std::move(other)) {}

    myself& operator++() {
        if (!this->m_pNode) return *this;
        
        // Si tiene hijo derecho, ir al mas izquierdo del hijo derecho
        if (this->m_pNode->getChild(1)) {
            this->m_pNode = this->m_pNode->getChild(1);
            while (this->m_pNode->getChild(0)) {
                this->m_pNode = this->m_pNode->getChild(0);
            }
        } else {
            // Subir hasta encontrar un padre donde vengamos del hijo izquierdo
            Node* parent = this->m_pNode->getParent();
            while (parent && this->m_pNode == parent->getChild(1)) {
                this->m_pNode = parent;
                parent = parent->getParent();
            }
            this->m_pNode = parent;
        }
        return *this;
    }
};

template <typename Container>
class binary_tree_backward_iterator: public general_iterator<Container, binary_tree_backward_iterator<Container>>{
public:
    typedef typename Container::Node Node;
    typedef binary_tree_backward_iterator<Container> myself;
    typedef general_iterator<Container, myself> Parent;
    // constructors
    binary_tree_backward_iterator(Container* pContainer, Node* pNode) : Parent(pContainer, pNode){}
    binary_tree_backward_iterator(const myself& other): Parent(other) {}
    binary_tree_backward_iterator(myself&& other) :Parent(std::move(other)){}

    myself& operator++(){
        if (!this->m_pNode) return *this;
        
        if (this->m_pNode->getChild(0)){
            this->m_pNode = this->m_pNode->getChild(0);
            while (this->m_pNode->getChild(1)){
                this->m_pNode = this->m_pNode->getChild(1);
            }
        }else{
            Node* pParent = this->m_pNode->getParent();
            while (pParent && this->m_pNode == pParent->getChild(0)){
                this->m_pNode = pParent;
                pParent = pParent->getParent();
            }
            this->m_pNode = pParent;
        }
        return *this;
    }

    myself operator++(int){
        myself temp = *this;
        ++(*this);
        return temp;
    }
};

template <typename _T>
struct BinaryTreeAscTraits {
    using T = _T;
    using Node = CBinaryTreeNode<BinaryTreeAscTraits<_T>>;
    using CompareFn = less<T>;
};

template <typename _T>
struct BinaryTreeDescTraits {
    using T = _T;
    using Node = CBinaryTreeNode<BinaryTreeDescTraits<_T>>;
    using CompareFn = greater<T>;
};

template <typename Traits>
class CBinaryTree {
public:
    typedef typename Traits::T value_type;
    typedef typename Traits::Node Node;
    typedef typename Traits::CompareFn CompareFn;
    typedef CBinaryTree<Traits> myself;
    typedef binary_tree_forward_iterator<myself> forward_iterator;
    typedef binary_tree_backward_iterator<myself> backward_iterator;
private:
    Node* m_pRoot;
    size_t m_size;
    CompareFn Compfn;
    mutable recursive_mutex m_mutex;
public: 
    // Constructor por defecto
    CBinaryTree() : m_pRoot(nullptr), m_size(0) {}
    
    // Copy Constructor
    CBinaryTree(const myself& other) : m_pRoot(nullptr), m_size(0) {
        lock_guard<recursive_mutex> lock(other.m_mutex);
        if (other.m_pRoot) {
            m_pRoot = copyTree(other.m_pRoot, nullptr);
            m_size = other.m_size;
        }
    }
    
    // Move Constructor
    CBinaryTree(myself&& other) : m_pRoot(nullptr), m_size(0) {
        lock_guard<recursive_mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_size = other.m_size;

        other.m_pRoot = nullptr;
        other.m_size = 0;
    }
    
    // Operador de asinacion
    myself& operator=(const myself& other) {
        if (this != &other) {
            scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            if (other.m_pRoot) {
                m_pRoot = copyTree(other.m_pRoot, nullptr);
                m_size = other.m_size;
            }
        }
        return *this;
    }
    
    // Destructor implementado
    virtual ~CBinaryTree() {
        lock_guard<recursive_mutex> lock(m_mutex);
        clear();
    }
    
    void clear() {
        lock_guard<recursive_mutex> lock(m_mutex);
        clearTree(m_pRoot);
        m_pRoot = nullptr;
        m_size = 0;
    }

    size_t size() const { 
        lock_guard<recursive_mutex> lock(m_mutex);
        return m_size; 
    }
    
    bool empty() const { 
        lock_guard<recursive_mutex> lock(m_mutex);
        return m_size == 0; 
    }
    
    // TODO: insert must receive two paramaters: elem and LinkedValueType value
    virtual void insert(value_type elem, Ref value ) { 
        internal_insert(elem, value, nullptr, m_pRoot);  
    }

    // Iterators
    // forward  ->
    forward_iterator begin(){
        lock_guard<recursive_mutex> lock(m_mutex);
        if(!m_pRoot) return forward_iterator(this, nullptr);
        // inicio : nodo mas a la izquierda
        Node* pNode = m_pRoot;
        while (pNode->getChild(0)){
            pNode = pNode->getChild(0);
        }
        return forward_iterator(this, pNode);
    }

    forward_iterator end(){
        return forward_iterator(this, nullptr);
    }

    //backward
    backward_iterator rbegin(){
        lock_guard<recursive_mutex> lock(m_mutex);
        if (!m_pRoot) return backward_iterator(this, nullptr);
        // inicio es el nodo + a la derecha
        Node* pNode = m_pRoot;
        while (pNode->getChild(1)){
            pNode = pNode->getChild(1);
        }
        return backward_iterator(this, pNode);
    }

    backward_iterator rend(){
        return backward_iterator(this, nullptr);

    }


protected:
    Node* CreateNode(Node* pParent, value_type elem, Ref ref) { 
        return new Node(pParent, elem, ref); 
    }
    
    Node* internal_insert(value_type elem, Ref ref, Node* pParent, Node*& rpOrigin) {
        if (!rpOrigin) {
            ++m_size;
            return (rpOrigin = CreateNode(pParent, elem, ref));
        }
        bool branch = Compfn(elem, rpOrigin->getDataRef());
        return internal_insert(elem, ref, rpOrigin, rpOrigin->getChildRef(branch ? 0 : 1));
    }
    
    Node* copyTree(Node* src, Node* parent) {
        if (!src) return nullptr;
        
        Node* newNode = new Node(parent, src->m_data, src->m_ref);
        newNode->m_pChild[0] = copyTree(src->m_pChild[0], newNode);
        newNode->m_pChild[1] = copyTree(src->m_pChild[1], newNode);
        return newNode;
    }
    
    void clearTree(Node* pNode) {
        if (!pNode) return;
        clearTree(pNode->getChild(0));
        clearTree(pNode->getChild(1));
        delete pNode;
    }

public:
    // initial functions
    void inorder(ostream& os) { 
        lock_guard<recursive_mutex> lock(m_mutex);
        inorder(m_pRoot, os, 0); 
    }
    void postorder(ostream& os) { 
        lock_guard<recursive_mutex> lock(m_mutex);
        postorder(m_pRoot, os, 0); 
    }
    void preorder(ostream& os) { 
        lock_guard<recursive_mutex> lock(m_mutex);
        preorder(m_pRoot, os, 0); 
    }
    void print(ostream& os) { 
        lock_guard<recursive_mutex> lock(m_mutex);
        //print(m_pRoot, os, 0); 
        inorder(os);
    }
    


    // variadic fucntions
    template<typename Function, typename... Args>
    void preorder_variadic(Function func, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        preorder_var(m_pRoot, func, std::forward<Args>(args)...);
    }
    
    template<typename Function, typename... Args>
    void inorder_variadic(Function func, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        inorder_var(m_pRoot, func, std::forward<Args>(args)...);
    }
    
    template<typename Function, typename... Args>
    void postorder_variadic(Function func, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        postorder_var(m_pRoot, func, std::forward<Args>(args)...);
    }

protected:
    void inorder(Node* pNode, ostream& os, size_t level) {
        if (pNode) {
            inorder(pNode->getChild(0), os, level + 1);
            os << " -> " << pNode->getData() << "(" << pNode->getDataRef() <<")";
            //os << " --> " << pNode->getData();
            inorder(pNode->getChild(1), os, level + 1);
        }
    }

    void postorder(Node* pNode, ostream& os, size_t level) {
        if (pNode) {
            postorder(pNode->getChild(0), os, level + 1);
            postorder(pNode->getChild(1), os, level + 1);
            //os << " --> " << pNode->getData();
            os << " -> " << pNode->getData() << "(" << pNode->getDataRef() <<")";
        }
    }

    void preorder(Node* pNode, ostream& os, size_t level) {
        if (pNode) {
            //os << " --> " << pNode->getData();
            os << " -> " << pNode->getData() << "(" << pNode->getDataRef() <<")";
            preorder(pNode->getChild(0), os, level + 1);
            preorder(pNode->getChild(1), os, level + 1);
        }
    }
    
    void print(Node* pNode, ostream& os, size_t level) {
        if (pNode) {
            print(pNode->getChild(1), os, level + 1);
            os << string(level * 6, ' ') << pNode->getData() 
               << "(" << (pNode->getParent() ? std::to_string(pNode->getDataRef()) : std::to_string(pNode->getDataRef())+")(Root") 
               << ")" << endl;
            print(pNode->getChild(0), os, level + 1);
        }
    }

    // variadic
    template<typename Function, typename... Args>
    void preorder_var(Node* pNode, Function func, Args&&... args) {
        if (pNode) {
            func(pNode->getDataRef(), std::forward<Args>(args)...);
            preorder_var(pNode->getChild(0), func, std::forward<Args>(args)...);
            preorder_var(pNode->getChild(1), func, std::forward<Args>(args)...);
        }
    }
    
    template<typename Function, typename... Args>
    void inorder_var(Node* pNode, Function func, Args&&... args) {
        if (pNode) {
            inorder_var(pNode->getChild(0), func, std::forward<Args>(args)...);
            func(pNode->getDataRef(), std::forward<Args>(args)...);
            inorder_var(pNode->getChild(1), func, std::forward<Args>(args)...);
        }
    }
    
    template<typename Function, typename... Args>
    void postorder_var(Node* pNode, Function func, Args&&... args) {
        if (pNode) {
            postorder_var(pNode->getChild(0), func, std::forward<Args>(args)...);
            postorder_var(pNode->getChild(1), func, std::forward<Args>(args)...);
            func(pNode->getDataRef(), std::forward<Args>(args)...);
        }
    }

public:
    // Serializacion
    // writes only in preorder
    void Write(ostream& os) {
        // preorder by default
        lock_guard< recursive_mutex> lock(m_mutex);
        //os << m_size << " ";
        preorder_variadic([&os](value_type& data) {
            os << data << " ";
        });
    }
    
    // read
    void Read(istream& is) {
        lock_guard< recursive_mutex> lock(m_mutex);
        clear();
        size_t count;
        is >> count;
        
        for (size_t i = 0; i < count; ++i) {
            value_type data;
            is >> data;
            insert(data);
        }
    }
    // reads in preorder/ such as write
    bool ReadFromFile(const string& filename) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file " << filename << " for reading" << endl;
            return false;
        }
        
        string line;
        if (!getline(file, line)) {
            cerr << "Error: Empty file" << endl;
            file.close();
            return false;
        }
        
        clear();
        

        vector<pair<value_type, Ref>> elements;
        stringstream ss(line);
        string item;
        
        while (ss >> item) {
            if (item == "->") continue;
            
            // Parsear "X(Y)"
            size_t openParen = item.find('(');
            size_t closeParen = item.find(')');
            
            if (openParen != string::npos && closeParen != string::npos) {
                try {
                    value_type value = static_cast<value_type>(stoi(item.substr(0, openParen)));
                    Ref ref = static_cast<Ref>(stol(item.substr(openParen + 1, closeParen - openParen - 1)));
                    elements.push_back({value, ref});
                } catch (...) {
                    // ignore non valid
                }
            }
        }
        
        // Reconstruir árbol insertando en el orden dado (asumiendo preorden)
        for (const auto& elem : elements) {
            insert(elem.first, elem.second);
        }
        
        file.close();
        cout << "Tree loaded from " << filename << " with " << m_size << " elements" << endl;
        return true;
}    

};

// <<operator 
template <typename Traits>
ostream& operator<<(ostream& os, CBinaryTree<Traits>& obj) {
    obj.print(os);
    return os;
}

template <typename Traits>
istream& operator>>(istream& is, CBinaryTree<Traits>& obj) {
    obj.Read(is);
    return is;
}

void DemoBinaryTree();

#endif // __BINARY_TREE_H__
