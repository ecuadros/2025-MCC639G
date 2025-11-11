#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include "binarytree.h"
#include "util.h"

using namespace std;
using namespace std::chrono;

mutex logMutex;

string getCurrentTimestamp() {
    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    
    stringstream ss;
    ss << put_time(localtime(&time_t_now), "%H:%M:%S");
    ss << "." << setfill('0') << setw(3) << ms.count();
    return ss.str();
}

void logMessage(const string& message) {
    lock_guard<mutex> lock(logMutex);
    cout << message << endl;
}

// Función para inserción concurrente
void insertarElementosBT(CBinaryTree<BinaryTreeAscTraits<T1>>& tree, T1 threadId, T1 start, T1 count) {
    for (T1 i = 0; i < count; i++) {
        T1 value = start + i;
        Ref ref = threadId * 100 + i;
        
        stringstream msg;
        msg << "[" << getCurrentTimestamp() << "] " 
            << "Hilo " << threadId << " INSERTANDO valor: " << value;
        logMessage(msg.str());
        
        tree.insert(value, ref);
        
        stringstream msg2;
        msg2 << "[" << getCurrentTimestamp() << "] " 
             << "Hilo " << threadId << " COMPLETADO valor: " << value;
        logMessage(msg2.str());
        
        this_thread::sleep_for(milliseconds(50));
    }
}

void TestBinaryTreeConcurrency() {
    cout << "\n=== TEST CONCURRENCIA ARBOL BINARIO ===" << endl;
    cout << "INICIO TEST: " << getCurrentTimestamp() << endl;
    
    CBinaryTree<BinaryTreeAscTraits<T1>> tree;
    vector<thread> threads;
    
    for (T1 i = 0; i < 3; i++) {
        threads.push_back(thread(insertarElementosBT, ref(tree), i, i * 10, 5));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "\nFIN INSERCIONES: " << getCurrentTimestamp() << endl;
    cout << "Tamaño final: " << tree.size() << endl;
    
    cout << "\n--- Recorridos ---" << endl;
    cout << "Inorder: ";
    tree.inorder(cout);
    cout << endl;
    
    cout << "Preorder: ";
    tree.preorder(cout);
    cout << endl;
    
    cout << "Postorder: ";
    tree.postorder(cout);
    cout << endl;
    
    cout << "=== FIN TEST CONCURRENCIA ===" << endl;
}

void TestBinaryTreeTraversals() {
    cout << "\n=== TEST RECORRIDOS ARBOL BINARIO ===" << endl;
    
    CBinaryTree<BinaryTreeAscTraits<T1>> tree;
    vector<pair<T1, Ref>> data = {
        {50, 1}, {30, 2}, {70, 3}, {20, 4}, {40, 5}, 
        {60, 6}, {80, 7}, {10, 8}, {25, 9}, {35, 10}
    };
    
    for (auto& [val, ref] : data) {
        tree.insert(val, ref);
    }
    
    cout << "Árbol completo:" << endl;
    cout << tree << endl;
    
    cout << "Recorrido Inorder: ";
    tree.inorder(cout);
    cout << endl;
    
    cout << "Recorrido Preorder: ";
    tree.preorder(cout);
    cout << endl;
    
    cout << "Recorrido Postorder: ";
    tree.postorder(cout);
    cout << endl;
    
    cout << "=== FIN TEST RECORRIDOS ===" << endl;
}

void TestBinaryTreeCopyAndMove() {
    cout << "\n=== TEST COPIA Y MOVIMIENTO ===" << endl;
    
    CBinaryTree<BinaryTreeAscTraits<T1>> tree1;
    vector<pair<T1, Ref>> data = {
        {50, 100}, {30, 200}, {70, 300}, {20, 400}, 
        {40, 500}, {60, 600}, {80, 700}
    };
    
    for (auto& [val, ref] : data) {
        tree1.insert(val, ref);
    }
    
    cout << "Árbol original:" << endl;
    cout << tree1 << endl;
    cout << "Tamaño: " << tree1.size() << endl;
    
    cout << "\n--- Test Constructor de Copia ---" << endl;
    CBinaryTree<BinaryTreeAscTraits<T1>> tree2(tree1);
    cout << "Original:" << endl;
    cout << tree1 << endl;
    cout << "Copia:" << endl;
    cout << tree2 << endl;
    
    tree2.insert(55, 999);
    cout << "Después de insertar en copia:" << endl;
    cout << "Original (debe permanecer igual):" << endl;
    cout << tree1 << endl;
    cout << "Copia (con nuevo elemento):" << endl;
    cout << tree2 << endl;
    
    cout << "\n--- Test Move Constructor ---" << endl;
    CBinaryTree<BinaryTreeAscTraits<T1>> tree3(move(tree1));
    cout << "Árbol movido:" << endl;
    cout << tree3 << endl;
    cout << "Árbol original después del move:" << endl;
    cout << tree1 << endl;
    cout << "¿Original vacío? " << (tree1.empty() ? "Sí" : "No") << endl;
    
    cout << "=== FIN TEST COPIA Y MOVIMIENTO ===" << endl;
}

void TestBinaryTreeDestructor() {
    cout << "\n=== TEST DESTRUCTOR ARBOL BINARIO ===" << endl;
    
    {
        CBinaryTree<BinaryTreeAscTraits<T1>> localTree;
        vector<pair<T1, Ref>> data = {
            {100, 1}, {50, 2}, {150, 3}, {25, 4}, {75, 5}
        };
        
        for (auto& [val, ref] : data) {
            localTree.insert(val, ref);
        }
        
        cout << "Árbol local antes de salir del scope:" << endl;
        cout << localTree << endl;
        cout << "Tamaño: " << localTree.size() << endl;
    }
    
    cout << "Scope terminado - destructor llamado exitosamente" << endl;
    cout << "=== FIN TEST DESTRUCTOR ===" << endl;
}

void TestBinaryTreeBasicOperations() {
    cout << "\n=== TEST OPERACIONES BASICAS ===" << endl;
    
    CBinaryTree<BinaryTreeAscTraits<T1>> tree;
    
    // Test inserción
    cout << "Insertando elementos..." << endl;
    tree.insert(50, 1);
    tree.insert(30, 2);
    tree.insert(70, 3);
    tree.insert(20, 4);
    tree.insert(40, 5);
    
    cout << "Árbol después de inserciones:" << endl;
    cout << tree << endl;
    cout << "Tamaño: " << tree.size() << endl;
    
    // Test recorridos
    cout << "Inorder: ";
    tree.inorder(cout);
    cout << endl;
    
    cout << "Preorder: ";
    tree.preorder(cout);
    cout << endl;
    
    cout << "Postorder: ";
    tree.postorder(cout);
    cout << endl;
    
    // Test clear
    cout << "\nLimpiando árbol..." << endl;
    tree.clear();
    cout << "Tamaño después de clear: " << tree.size() << endl;
    cout << "¿Árbol vacío? " << (tree.empty() ? "Sí" : "No") << endl;
    
    cout << "=== FIN TEST OPERACIONES BASICAS ===" << endl;
}

void DemoBinaryTree() {
    cout << "\n=== DEMO ARBOL BINARIO ===" << endl;
    
    CBinaryTree<BinaryTreeAscTraits<T1>> tree;
    vector<pair<T1, Ref>> data = {
        {8, 1}, {3, 2}, {10, 3}, {1, 4}, {6, 5}, {14, 6}, {4, 7}, {7, 8}
    };
    
    for (auto& [val, ref] : data) {
        tree.insert(val, ref);
    }
    
    cout << "Árbol binario completo:" << endl;
    cout << tree << endl;
    
    cout << "Recorrido Inorder: ";
    tree.inorder(cout);
    cout << endl;
    
    cout << "Recorrido Preorder: ";
    tree.preorder(cout);
    cout << endl;
    
    cout << "Recorrido Postorder: ";
    tree.postorder(cout);
    cout << endl;
    
    cout << "Tamaño del árbol: " << tree.size() << endl;
    
    // Ejecutar todos los tests
    TestBinaryTreeBasicOperations();
    TestBinaryTreeTraversals();
    TestBinaryTreeConcurrency();
    TestBinaryTreeCopyAndMove();
    TestBinaryTreeDestructor();
}