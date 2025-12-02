#ifndef __AVL_H__
#define __AVL_H__

#include <iostream>
#include <stack>
#include <queue>
#include <mutex>
#include <algorithm>
using namespace std;


template <typename T>
struct BTNode {
    T value;
    BTNode* left;
    BTNode* right;
    int height;

    BTNode(const T& v)
        : value(v), left(nullptr), right(nullptr), height(1) {}
};




template <typename T>
class AVLTree {
private:
    BTNode<T>* root;
    size_t n;
    mutable std::mutex mtx;

public:

 
    // Constructor / Destructor

    AVLTree() : root(nullptr), n(0) {}

    ~AVLTree() {
        clear(root);
    }


    // Insertar con balance (AVL) 

    void insert(const T& value) {
        lock_guard<mutex> lock(mtx);
        root = insertRec(root, value);
    }

private:

    int height(BTNode<T>* node) {
        return node ? node->height : 0;
    }

    int balanceFactor(BTNode<T>* node) {
        return height(node->left) - height(node->right);
    }

    void updateHeight(BTNode<T>* node) {
        node->height = 1 + max(height(node->left), height(node->right));
    }

    BTNode<T>* rotateRight(BTNode<T>* y) {
        BTNode<T>* x = y->left;
        BTNode<T>* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    BTNode<T>* rotateLeft(BTNode<T>* x) {
        BTNode<T>* y = x->right;
        BTNode<T>* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    BTNode<T>* insertRec(BTNode<T>* node, const T& value) {
        if (!node) {
            n++;
            return new BTNode<T>(value);
        }

        if (value < node->value)
            node->left = insertRec(node->left, value);
        else if (value > node->value)
            node->right = insertRec(node->right, value);
        else
            return node; // evitar duplicados

        updateHeight(node);

        int bf = balanceFactor(node);

        //      CASOS AVL
       

        // Left Left
        if (bf > 1 && value < node->left->value)
            return rotateRight(node);

        // Right Right
        if (bf < -1 && value > node->right->value)
            return rotateLeft(node);

        // Left Right
        if (bf > 1 && value > node->left->value) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right Left
        if (bf < -1 && value < node->right->value) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void clear(BTNode<T>* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }


public:

    // Compatible con foreach 
    // (forward iterator inorden)

    class iterator {
        stack<BTNode<T>*> st;

        void pushLeft(BTNode<T>* n) {
            while (n) {
                st.push(n);
                n = n->left;
            }
        }

    public:
        iterator(BTNode<T>* root) { pushLeft(root); }

        bool operator!=(const iterator& other) const {
            return st != other.st;
        }

        const T& operator*() const {
            return st.top()->value;
        }

        iterator& operator++() {
            BTNode<T>* n = st.top(); st.pop();
            pushLeft(n->right);
            return *this;
        }
    };

    iterator begin() const { return iterator(root); }
    iterator end()   const { return iterator(nullptr); }



    // Compatible con recorridos 


    template <typename F>
    void preorder(F f) const {
        preorderRec(root, f);
    }

    template <typename F>
    void inorder(F f) const {
        inorderRec(root, f);
    }

    template <typename F>
    void postorder(F f) const {
        postorderRec(root, f);
    }

private:

    template <typename F>
    void preorderRec(BTNode<T>* node, F& f) const {
        if (!node) return;
        f(node->value);
        preorderRec(node->left, f);
        preorderRec(node->right, f);
    }

    template <typename F>
    void inorderRec(BTNode<T>* node, F& f) const {
        if (!node) return;
        inorderRec(node->left, f);
        f(node->value);
        inorderRec(node->right, f);
    }

    template <typename F>
    void postorderRec(BTNode<T>* node, F& f) const {
        if (!node) return;
        postorderRec(node->left, f);
        postorderRec(node->right, f);
        f(node->value);
    }


public:

 
    // Compatible Write, Read 

    friend ostream& operator<<(ostream& os, const AVLTree& bt) {
        os << "{ ";
        bt.inorder([&](const T& v) { os << v << " "; });
        os << "}";
        return os;
    }

    friend istream& operator>>(istream& is, AVLTree& bt) {
        T value;
        while (is >> value)
            bt.insert(value);
        return is;
    }


    // Utilidad

    void print() const {
        inorder([](const T& v){
            cout << v << " ";
        });
        cout << "\n";
    }

};

#endif // __AVL_H__
