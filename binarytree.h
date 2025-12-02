#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <iostream>
#include <stack>
#include <queue>
#include <initializer_list>
#include <mutex>    // Usado para Concurrence

using namespace std;

template <typename T>
class BinaryTree {
public:


    //  Nodo Interno
 
    struct Node {
        T value;
        Node* left;
        Node* right;

        Node(const T& v) : value(v), left(nullptr), right(nullptr) {}
    };

private:
    Node* root;
    size_t n;
    mutable std::mutex mtx; // para Concurrence

public:

   
    // Constructor Copia

    BinaryTree(const BinaryTree& other) : root(nullptr), n(0) {
        std::lock_guard<std::mutex> lock(other.mtx);
        if (other.root)
            root = copySubtree(other.root);
        n = other.n;
    }

 
    //  Move Constructor

    BinaryTree(BinaryTree&& other) noexcept 
        : root(other.root), n(other.n) 
    {
        other.root = nullptr;
        other.n = 0;
    }


    // Constructor Principal
  
    BinaryTree() : root(nullptr), n(0) {}


    // Destructor

    ~BinaryTree() {
        clear(root);
    }

private:

    // Copia recursiva
    Node* copySubtree(Node* node) {
        if (!node) return nullptr;
        Node* newNode = new Node(node->value);
        newNode->left = copySubtree(node->left);
        newNode->right = copySubtree(node->right);
        return newNode;
    }

    // Liberar memoria
    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:


    // Insert
    void insert(const T& value) {
        lock_guard<mutex> lock(mtx);
        root = insertRec(root, value);
        n++;
    }

private:
    Node* insertRec(Node* node, const T& value) {
        if (!node) return new Node(value);

        if (value < node->value)
            node->left = insertRec(node->left, value);
        else
            node->right = insertRec(node->right, value);

        return node;
    }

public:

 
    // forward iterator
 
    class iterator {
        stack<Node*> st;

        void pushLeft(Node* n) {
            while (n) {
                st.push(n);
                n = n->left;
            }
        }

    public:
        iterator(Node* root) {
            pushLeft(root);
        }

        bool operator!=(const iterator& other) const {
            return st != other.st;
        }

        const T& operator*() const {
            return st.top()->value;
        }

        iterator& operator++() {
            Node* n = st.top(); st.pop();
            pushLeft(n->right);
            return *this;
        }
    };

    iterator begin() const { return iterator(root); }
    iterator end()   const { return iterator(nullptr); }


    // backward iterator

    class reverse_iterator {
        stack<Node*> st;

        void pushRight(Node* n) {
            while (n) {
                st.push(n);
                n = n->right;
            }
        }

    public:
        reverse_iterator(Node* root) {
            pushRight(root);
        }

        bool operator!=(const reverse_iterator& other) const {
            return st != other.st;
        }

        const T& operator*() const {
            return st.top()->value;
        }

        reverse_iterator& operator++() {
            Node* n = st.top(); st.pop();
            pushRight(n->left);
            return *this;
        }
    };

    reverse_iterator rbegin() const { return reverse_iterator(root); }
    reverse_iterator rend()   const { return reverse_iterator(nullptr); }


    // preorder (Variadic)

    template <typename F>
    void preorder(F f) const {
        preorderRec(root, f);
    }

    template <typename F>
    void preorderRec(Node* node, F& f) const {
        if (!node) return;
        f(node->value);
        preorderRec(node->left, f);
        preorderRec(node->right, f);
    }


    // inorder (Variadic)

    template <typename F>
    void inorder(F f) const {
        inorderRec(root, f);
    }

private:
    template <typename F>
    void inorderRec(Node* node, F& f) const {
        if (!node) return;
        inorderRec(node->left, f);
        f(node->value);
        inorderRec(node->right, f);
    }

public:

    // postorder (Variadic)

    template <typename F>
    void postorder(F f) const {
        postorderRec(root, f);
    }

private:
    template <typename F>
    void postorderRec(Node* node, F& f) const {
        if (!node) return;
        postorderRec(node->left, f);
        postorderRec(node->right, f);
        f(node->value);
    }

public:

 
    //  Write y operador <<
 
    friend ostream& operator<<(ostream& os, const BinaryTree& bt) {
        os << "{ ";
        bt.inorder([&](const T& v) { os << v << " "; });
        os << "}";
        return os;
    }

 
    // Read y operador >>

    friend istream& operator>>(istream& is, BinaryTree& bt) {
        T value;
        while (is >> value)
            bt.insert(value);
        return is;
    }

    //  print reutiliza inorden
    void print() const {
        inorder([](const T& v) {
            cout << v << " ";
        });
        cout << "\n";
    }

   
    // Concurrence 
    size_t size() const {
        lock_guard<mutex> lock(mtx);
        return n;
    }

    
};

#endif
