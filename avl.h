#ifndef __AVL_H__
#define __AVL_H__

#include <stack>
#include <queue>
#include <fstream>
#include <iostream>
#include "types.h"


#include "types.h"

template <typename T1, typename Ref>
class AVLNode {
public:
    T1 data;
    Ref reference;
    AVLNode* left;
    AVLNode* right;
    int height;
    int balanceFactor;
    
    AVLNode(const T1& value, const Ref& ref) 
        : data(value), reference(ref), left(nullptr), right(nullptr), height(1), balanceFactor(0) {}
    
    AVLNode(const T1& value)
        : data(value), reference(Ref()), left(nullptr), right(nullptr), height(1), balanceFactor(0) {}
    
    ~AVLNode() {
        delete left;
        delete right;
    }
    
    // Métodos para acceso seguro
    bool hasLeft() const { return left != nullptr; }
    bool hasRight() const { return right != nullptr; }
    bool isLeaf() const { return !hasLeft() && !hasRight(); }
    
    // Actualizar altura y factor de balance
    void updateHeight() {
        int leftHeight = left ? left->height : 0;
        int rightHeight = right ? right->height : 0;
        height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
        balanceFactor = leftHeight - rightHeight;
    }
};

template <typename T1, typename Ref, typename OrderTraits = AscendingTraits>
class AVLTree {
private:
    typedef AVLNode<T1, Ref> Node;
    Node* root;
    size_t m_size;
    
    // Utilidades de altura
    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }
    
    void updateHeight(Node* node) {
        if (node) {
            node->updateHeight();
        }
    }
    
    // Rotaciones
    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    // Comparación basada en traits de ordenamiento
    bool shouldGoLeft(const T1& newValue, const T1& nodeValue) const {
        if (OrderTraits::ascending) {
            return newValue < nodeValue;
        } else {
            return newValue > nodeValue;
        }
    }
    
    bool shouldGoRight(const T1& newValue, const T1& nodeValue) const {
        if (OrderTraits::ascending) {
            return newValue > nodeValue;
        } else {
            return newValue < nodeValue;
        }
    }
    
    // Inserción con balanceo
    Node* insert(Node* node, const T1& value, const Ref& ref) {
        if (!node) {
            m_size++;
            return new Node(value, ref);
        }
        
        if (shouldGoLeft(value, node->data)) {
            node->left = insert(node->left, value, ref);
        } else if (shouldGoRight(value, node->data)) {
            node->right = insert(node->right, value, ref);
        } else {
            // Duplicado - actualizar referencia
            node->reference = ref;
            return node;
        }
        
        updateHeight(node);
        
        int balance = node->balanceFactor;
        
        // Casos de rotación
        if (balance > 1) {
            if (shouldGoLeft(value, node->left->data)) {
                // Left Left Case
                return rightRotate(node);
            } else {
                // Left Right Case
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }
        
        if (balance < -1) {
            if (shouldGoRight(value, node->right->data)) {
                // Right Right Case
                return leftRotate(node);
            } else {
                // Right Left Case
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }
        
        return node;
    }
    
    // Encontrar nodo mínimo/máximo según ordenamiento
    Node* findExtreme(Node* node) const {
        if (!node) return nullptr;
        
        if (OrderTraits::ascending) {
            while (node->left) {
                node = node->left;
            }
        } else {
            while (node->right) {
                node = node->right;
            }
        }
        return node;
    }
    
    // Eliminación con balanceo
    Node* remove(Node* node, const T1& value) {
        if (!node) return nullptr;
        
        if (shouldGoLeft(value, node->data)) {
            node->left = remove(node->left, value);
        } else if (shouldGoRight(value, node->data)) {
            node->right = remove(node->right, value);
        } else {
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp; // Copy contents
                    temp->left = temp->right = nullptr; // Prevent double deletion
                }
                delete temp;
                m_size--;
            } else {
                Node* temp;
                if (OrderTraits::ascending) {
                    temp = findExtreme(node->right);
                } else {
                    temp = findExtreme(node->left);
                }
                node->data = temp->data;
                node->reference = temp->reference;
                
                if (OrderTraits::ascending) {
                    node->right = remove(node->right, temp->data);
                } else {
                    node->left = remove(node->left, temp->data);
                }
            }
        }
        
        if (!node) return nullptr;
        
        updateHeight(node);
        
        int balance = node->balanceFactor;
        
        // Balancear después de eliminar
        if (balance > 1) {
            if (node->left->balanceFactor >= 0) {
                return rightRotate(node);
            } else {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }
        
        if (balance < -1) {
            if (node->right->balanceFactor <= 0) {
                return leftRotate(node);
            } else {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }
        
        return node;
    }
    
    // Helpers para recorridos
    template<typename Function>
    void inorder(Node* node, Function& func) const {
        if (!node) return;
        inorder(node->left, func);
        func(node->data, node->reference);
        inorder(node->right, func);
    }
    
    template<typename Function>
    void reverseInorder(Node* node, Function& func) const {
        if (!node) return;
        reverseInorder(node->right, func);
        func(node->data, node->reference);
        reverseInorder(node->left, func);
    }
    
    template<typename Function>
    void preorder(Node* node, Function& func) const {
        if (!node) return;
        func(node->data, node->reference);
        preorder(node->left, func);
        preorder(node->right, func);
    }
    
    template<typename Function>
    void postorder(Node* node, Function& func) const {
        if (!node) return;
        postorder(node->left, func);
        postorder(node->right, func);
        func(node->data, node->reference);
    }
    
    // Copiar árbol (para operador de asignación)
    Node* copyTree(Node* other) {
        if (!other) return nullptr;
        
        Node* newNode = new Node(other->data, other->reference);
        newNode->height = other->height;
        newNode->balanceFactor = other->balanceFactor;
        newNode->left = copyTree(other->left);
        newNode->right = copyTree(other->right);
        
        return newNode;
    }
    
    // Limpiar árbol
    void clearTree(Node* node) {
        if (node) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr), m_size(0) {}
    
    // Rule of Three
    AVLTree(const AVLTree& other) : root(nullptr), m_size(0) {
        root = copyTree(other.root);
        m_size = other.m_size;
    }
    
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clearTree(root);
            root = copyTree(other.root);
            m_size = other.m_size;
        }
        return *this;
    }
    
    ~AVLTree() {
        clearTree(root);
    }
    
    // Operaciones básicas
    void insert(const T1& value, const Ref& ref) {
        root = insert(root, value, ref);
    }
    
    void insert(const T1& value) {
        insert(value, Ref());
    }
    
    bool remove(const T1& value) {
        size_t oldSize = m_size;
        root = remove(root, value);
        return m_size != oldSize;
    }
    
    bool search(const T1& value) const {
        Node* current = root;
        while (current) {
            if (value == current->data) {
                return true;
            } else if (shouldGoLeft(value, current->data)) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return false;
    }
    
    bool getReference(const T1& value, Ref& ref) const {
        Node* current = root;
        while (current) {
            if (value == current->data) {
                ref = current->reference;
                return true;
            } else if (shouldGoLeft(value, current->data)) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return false;
    }
    
    // Métodos de recorrido
    template<typename Function>
    void traverseInorder(Function func) const {
        if (OrderTraits::ascending) {
            inorder(root, func);
        } else {
            reverseInorder(root, func);
        }
    }
    
    template<typename Function>
    void traversePreorder(Function func) const {
        preorder(root, func);
    }
    
    template<typename Function>
    void traversePostorder(Function func) const {
        postorder(root, func);
    }
    
    template<typename Function>
    void traverseLevelorder(Function func) const {
        if (!root) return;
        
        std::queue<Node*> q;
        q.push(root);
        
        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            func(current->data, current->reference);
            
            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
    }
    
    // Mínimo/Máximo según ordenamiento
    T1 findExtreme() const {
        if (!root) throw std::runtime_error("Tree is empty");
        return findExtreme(root)->data;
    }
    
    // Utilidades
    bool isBalanced() const {
        return isBalanced(root);
    }
    
    bool isBalanced(Node* node) const {
        if (!node) return true;
        
        int balance = node->balanceFactor;
        if (balance < -1 || balance > 1) return false;
        
        return isBalanced(node->left) && isBalanced(node->right);
    }
    
    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }
    
    // Soporte para iteradores
    class Iterator {
    private:
        std::stack<Node*> stack;
        bool ascending;
        
        void pushLeft(Node* node) {
            while (node) {
                stack.push(node);
                node = ascending ? node->left : node->right;
            }
        }
        
    public:
        Iterator(Node* root, bool asc = true) : ascending(asc) {
            pushLeft(root);
        }
        
        std::pair<T1, Ref> operator*() {
            Node* node = stack.top();
            return std::make_pair(node->data, node->reference);
        }
        
        Iterator& operator++() {
            if (stack.empty()) return *this;
            
            Node* node = stack.top();
            stack.pop();
            pushLeft(ascending ? node->right : node->left);
            return *this;
        }
        
        bool operator!=(const Iterator& other) const {
            if (stack.empty() && other.stack.empty()) return false;
            if (stack.empty() || other.stack.empty()) return true;
            return stack.top() != other.stack.top();
        }
    };
    
    Iterator begin() const { 
        return Iterator(root, OrderTraits::ascending); 
    }
    
    Iterator end() const { 
        return Iterator(nullptr, OrderTraits::ascending); 
    }
    
    // Escritura y Lectura con traits
    bool writeToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        // Escribir metadata
        file.write(reinterpret_cast<const char*>(&m_size), sizeof(m_size));
        
        // Escribir datos en el orden correspondiente
        traverseInorder([&file](const T1& data, const Ref& ref) {
            file.write(reinterpret_cast<const char*>(&data), sizeof(T1));
            file.write(reinterpret_cast<const char*>(&ref), sizeof(Ref));
        });
        
        return file.good();
    }
    
    bool readFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        // Limpiar árbol actual
        clearTree(root);
        root = nullptr;
        m_size = 0;
        
        // Leer metadata
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        
        // Reconstruir árbol
        for (size_t i = 0; i < count; ++i) {
            T1 data;
            Ref ref;
            file.read(reinterpret_cast<char*>(&data), sizeof(T1));
            file.read(reinterpret_cast<char*>(&ref), sizeof(Ref));
            
            if (file.good()) {
                insert(data, ref);
            }
        }
        
        return file.eof() || file.good();
    }
    
    // Método para demostración
    void printTree() const {
        std::cout << "AVL Tree (" << OrderTraits::name() << "): " << m_size << " elements\n";
        traverseInorder([](const T1& data, const Ref& ref) {
            std::cout << "Data: " << data << ", Ref: " << ref << std::endl;
        });
    }
    
    // Limpiar todo el árbol
    void clear() {
        clearTree(root);
        root = nullptr;
        m_size = 0;
    }
};

// Alias para árboles ascendentes y descendentes
template<typename T1, typename Ref>
using AscendingAVL = AVLTree<T1, Ref, AscendingTraits>;

template<typename T1, typename Ref>
using DescendingAVL = AVLTree<T1, Ref, DescendingTraits>;

void DemoAVL();

#endif // __AVL_H__





////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


template <typename T>
class AVLTree {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height;
        int balanceFactor;
        
        Node(const T& value) : data(value), left(nullptr), right(nullptr), height(1), balanceFactor(0) {}
    };
    
    std::unique_ptr<Node> root;
    size_t m_size = 0;
    
    // Height and balance utilities
    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }
    
    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left.get()), getHeight(node->right.get()));
            node->balanceFactor = getHeight(node->left.get()) - getHeight(node->right.get());
        }
    }
    
    // Rotations
    std::unique_ptr<Node> rightRotate(std::unique_ptr<Node> y) {
        auto x = std::move(y->left);
        auto T2 = std::move(x->right);
        
        x->right = std::move(y);
        x->right->left = std::move(T2);
        
        updateHeight(x->right.get());
        updateHeight(x.get());
        
        return x;
    }
    
    std::unique_ptr<Node> leftRotate(std::unique_ptr<Node> x) {
        auto y = std::move(x->right);
        auto T2 = std::move(y->left);
        
        y->left = std::move(x);
        y->left->right = std::move(T2);
        
        updateHeight(y->left.get());
        updateHeight(y.get());
        
        return y;
    }
    
    // Insert with balancing
    std::unique_ptr<Node> insert(std::unique_ptr<Node> node, const T& value) {
        if (!node) {
            m_size++;
            return std::make_unique<Node>(value);
        }
        
        if (value < node->data) {
            node->left = insert(std::move(node->left), value);
        } else if (value > node->data) {
            node->right = insert(std::move(node->right), value);
        } else {
            return node; // Duplicate
        }
        
        updateHeight(node.get());
        
        int balance = node->balanceFactor;
        
        // Left Left Case
        if (balance > 1 && value < node->left->data) {
            return rightRotate(std::move(node));
        }
        
        // Right Right Case
        if (balance < -1 && value > node->right->data) {
            return leftRotate(std::move(node));
        }
        
        // Left Right Case
        if (balance > 1 && value > node->left->data) {
            node->left = leftRotate(std::move(node->left));
            return rightRotate(std::move(node));
        }
        
        // Right Left Case
        if (balance < -1 && value < node->right->data) {
            node->right = rightRotate(std::move(node->right));
            return leftRotate(std::move(node));
        }
        
        return node;
    }
    
    // Find minimum node
    Node* findMin(Node* node) const {
        while (node && node->left) {
            node = node->left.get();
        }
        return node;
    }
    
    // Delete with balancing
    std::unique_ptr<Node> remove(std::unique_ptr<Node> node, const T& value) {
        if (!node) return nullptr;
        
        if (value < node->data) {
            node->left = remove(std::move(node->left), value);
        } else if (value > node->data) {
            node->right = remove(std::move(node->right), value);
        } else {
            if (!node->left || !node->right) {
                node = std::move(node->left ? node->left : node->right);
                m_size--;
            } else {
                Node* temp = findMin(node->right.get());
                node->data = temp->data;
                node->right = remove(std::move(node->right), temp->data);
            }
        }
        
        if (!node) return nullptr;
        
        updateHeight(node.get());
        
        int balance = node->balanceFactor;
        
        // Left Left Case
        if (balance > 1 && node->left->balanceFactor >= 0) {
            return rightRotate(std::move(node));
        }
        
        // Left Right Case
        if (balance > 1 && node->left->balanceFactor < 0) {
            node->left = leftRotate(std::move(node->left));
            return rightRotate(std::move(node));
        }
        
        // Right Right Case
        if (balance < -1 && node->right->balanceFactor <= 0) {
            return leftRotate(std::move(node));
        }
        
        // Right Left Case
        if (balance < -1 && node->right->balanceFactor > 0) {
            node->right = rightRotate(std::move(node->right));
            return leftRotate(std::move(node));
        }
        
        return node;
    }
    
    // Traversal helpers
    template<typename Function>
    void inorder(Node* node, Function& func) const {
        if (!node) return;
        inorder(node->left.get(), func);
        func(node->data);
        inorder(node->right.get(), func);
    }
    
    template<typename Function>
    void preorder(Node* node, Function& func) const {
        if (!node) return;
        func(node->data);
        preorder(node->left.get(), func);
        preorder(node->right.get(), func);
    }
    
    template<typename Function>
    void postorder(Node* node, Function& func) const {
        if (!node) return;
        postorder(node->left.get(), func);
        postorder(node->right.get(), func);
        func(node->data);
    }
    
    // Balance checking
    bool isBalanced(Node* node) const {
        if (!node) return true;
        
        int balance = node->balanceFactor;
        if (balance < -1 || balance > 1) return false;
        
        return isBalanced(node->left.get()) && isBalanced(node->right.get());
    }

public:
    AVLTree() = default;
    
    // Basic operations
    void insert(const T& value) {
        root = insert(std::move(root), value);
    }
    
    bool remove(const T& value) {
        size_t oldSize = m_size;
        root = remove(std::move(root), value);
        return m_size != oldSize;
    }
    
    bool search(const T& value) const {
        Node* current = root.get();
        while (current) {
            if (value < current->data) {
                current = current->left.get();
            } else if (value > current->data) {
                current = current->right.get();
            } else {
                return true;
            }
        }
        return false;
    }
    
    // Traversal methods
    template<typename Function>
    void traverseInorder(Function func) const {
        inorder(root.get(), func);
    }
    
    template<typename Function>
    void traversePreorder(Function func) const {
        preorder(root.get(), func);
    }
    
    template<typename Function>
    void traversePostorder(Function func) const {
        postorder(root.get(), func);
    }
    
    template<typename Function>
    void traverseLevelorder(Function func) const {
        if (!root) return;
        
        std::queue<Node*> q;
        q.push(root.get());
        
        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            func(current->data);
            
            if (current->left) q.push(current->left.get());
            if (current->right) q.push(current->right.get());
        }
    }
    
    // Min/Max
    T findMin() const {
        if (!root) throw std::runtime_error("Tree is empty");
        return findMin(root.get())->data;
    }
    
    T findMax() const {
        if (!root) throw std::runtime_error("Tree is empty");
        Node* current = root.get();
        while (current->right) {
            current = current->right.get();
        }
        return current->data;
    }
    
    // Utility methods
    bool isBalanced() const {
        return isBalanced(root.get());
    }
    
    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }
    
    // Iterator support
    class Iterator {
    private:
        std::stack<Node*> stack;
        
        void pushLeft(Node* node) {
            while (node) {
                stack.push(node);
                node = node->left.get();
            }
        }
        
    public:
        Iterator(Node* root) {
            pushLeft(root);
        }
        
        T& operator*() {
            return stack.top()->data;
        }
        
        Iterator& operator++() {
            Node* node = stack.top();
            stack.pop();
            pushLeft(node->right.get());
            return *this;
        }
        
        bool operator!=(const Iterator& other) const {
            return !stack.empty() || !other.stack.empty();
        }
    };
    
    Iterator begin() const { return Iterator(root.get()); }
    Iterator end() const { return Iterator(nullptr); }
    
    // File I/O
    bool writeToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        // Simple serialization - you can enhance this
        file.write(reinterpret_cast<const char*>(&m_size), sizeof(m_size));
        traversePreorder([&file](const T& value) {
            file.write(reinterpret_cast<const char*>(&value), sizeof(T));
        });
        
        file.close();
        return true;
    }
    
    bool readFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        root.reset();
        m_size = 0;
        
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        
        for (size_t i = 0; i < count; ++i) {
            T value;
            file.read(reinterpret_cast<char*>(&value), sizeof(T));
            insert(value);
        }
        
        file.close();
        return true;
    }
};
void DemoAVL();
#endif // __AVL_H__