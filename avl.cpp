#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "avl.h"
#include "util.h"

using namespace std;
using namespace std::chrono;



void TestAVLBasicOperations() {
    cout << "\n=== TEST OPERACIONES BASICAS AVL ===" << endl;
    
    CAVLTree<AVLAscTraits<T1>> tree;
    
    // Insertar elementos que causarían desbalance en un BST normal
    vector<pair<T1, Ref>> data = {
        {10, 1}, {20, 2}, {30, 3}, {40, 4}, {50, 5}, {25, 6}
    };
    
    cout << "Insertando elementos en orden que requiere balanceo..." << endl;
    for (auto& [val, ref] : data) {
        tree.insert(val, ref);
        cout << "Después de insertar " << val << ":" << endl;
        cout << tree << endl;
        cout << "¿Balanceado? " << (tree.isBalanced() ? "Sí" : "No") << endl;
        cout << "Tamaño: " << tree.size() << endl;
        cout << "---" << endl;
    }
    
    cout << "Árbol AVL final:" << endl;
    cout << tree << endl;
    cout << "Tamaño: " << tree.size() << endl;
    cout << "¿Balanceado? " << (tree.isBalanced() ? "Sí" : "No") << endl;
    
    // Test recorridos
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
    
    cout << "=== FIN TEST OPERACIONES BASICAS AVL ===" << endl;
}

void TestAVLConcurrency() {
    cout << "\n=== TEST CONCURRENCIA AVL ===" << endl;
    cout << "INICIO TEST: " << getCurrentTimestamp() << endl;
    
    CAVLTree<AVLAscTraits<T1>> tree;
    vector<thread> threads;
    
    // Función para inserción concurrente
    auto insertarElementosAVL = [](CAVLTree<AVLAscTraits<T1>>& tree, T1 threadId, T1 start, T1 count) {
        for (T1 i = 0; i < count; i++) {
            T1 value = start + i;
            Ref ref = threadId * 100 + i;
            
            stringstream msg;
            msg << "[" << getCurrentTimestamp() << "] " 
                << "Hilo " << threadId << " INSERTANDO AVL valor: " << value;
            logMessage(msg.str());
            
            tree.insert(value, ref);
            
            stringstream msg2;
            msg2 << "[" << getCurrentTimestamp() << "] " 
                 << "Hilo " << threadId << " COMPLETADO AVL valor: " << value;
            logMessage(msg2.str());
            
            this_thread::sleep_for(milliseconds(30));
        }
    };
    
    for (T1 i = 0; i < 3; i++) {
        threads.push_back(thread(insertarElementosAVL, ref(tree), i, i * 20, 4));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "\nFIN INSERCIONES AVL: " << getCurrentTimestamp() << endl;
    cout << "Tamaño final: " << tree.size() << endl;
    cout << "¿Balanceado? " << (tree.isBalanced() ? "Sí" : "No") << endl;
    
    cout << "\n--- Recorridos AVL ---" << endl;
    cout << "Inorder: ";
    tree.inorder(cout);
    cout << endl;
    
    cout << "=== FIN TEST CONCURRENCIA AVL ===" << endl;
}

void TestAVLCopyAndMove() {
    cout << "\n=== TEST COPIA Y MOVIMIENTO AVL ===" << endl;
    
    CAVLTree<AVLAscTraits<T1>> tree1;
    vector<pair<T1, Ref>> data = {
        {50, 1}, {30, 2}, {70, 3}, {20, 4}, {40, 5}, {60, 6}, {80, 7}
    };
    
    for (auto& [val, ref] : data) {
        tree1.insert(val, ref);
    }
    
    cout << "Árbol AVL original:" << endl;
    cout << tree1 << endl;
    cout << "Tamaño: " << tree1.size() << endl;
    cout << "¿Balanceado? " << (tree1.isBalanced() ? "Sí" : "No") << endl;
    
    cout << "\n--- Test Constructor de Copia AVL ---" << endl;
    CAVLTree<AVLAscTraits<T1>> tree2(tree1);
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
    cout << "¿Copia balanceada? " << (tree2.isBalanced() ? "Sí" : "No") << endl;
    
    cout << "=== FIN TEST COPIA Y MOVIMIENTO AVL ===" << endl;
}

void TestAVLBalanceScenarios() {
    cout << "\n=== TEST ESCENARIOS DE BALANCEO AVL ===" << endl;
    
    // Test 1: Inserción ascendente (peor caso para BST normal)
    cout << "Test 1: Inserción ascendente" << endl;
    CAVLTree<AVLAscTraits<T1>> tree1;
    for (T1 i = 1; i <= 7; i++) {
        tree1.insert(i * 10, i);
        cout << "Insertado " << (i * 10) << " - Balanceado: " << (tree1.isBalanced() ? "Sí" : "No") << endl;
    }
    cout << "Árbol después de inserciones ascendentes:" << endl;
    cout << tree1 << endl;
    cout << "¿Balanceado? " << (tree1.isBalanced() ? "Sí" : "No") << endl;
    cout << "Tamaño: " << tree1.size() << endl;
    
    // Test 2: Inserción descendente
    cout << "\nTest 2: Inserción descendente" << endl;
    CAVLTree<AVLAscTraits<T1>> tree2;
    for (T1 i = 7; i >= 1; i--) {
        tree2.insert(i * 10, i);
        cout << "Insertado " << (i * 10) << " - Balanceado: " << (tree2.isBalanced() ? "Sí" : "No") << endl;
    }
    cout << "Árbol después de inserciones descendentes:" << endl;
    cout << tree2 << endl;
    cout << "¿Balanceado? " << (tree2.isBalanced() ? "Sí" : "No") << endl;
    
    cout << "=== FIN TEST ESCENARIOS DE BALANCEO AVL ===" << endl;
}

void DemoAVLTree() {
    cout << "\n=== DEMO ARBOL AVL ===" << endl;
    
    CAVLTree<AVLAscTraits<T1>> tree;
    vector<pair<T1, Ref>> data = {
        {10, 1}, {20, 2}, {30, 3}, {40, 4}, {50, 5}, 
        {25, 6}, {35, 7}, {45, 8}, {15, 9}
    };
    
    cout << "Insertando elementos con balanceo automático..." << endl;
    for (auto& [val, ref] : data) {
        cout << "Insertando: " << val << endl;
        tree.insert(val, ref);
        cout << "Árbol actual:" << endl;
        cout << tree << endl;
        cout << "Balanceado: " << (tree.isBalanced() ? "Sí" : "No") << endl;
        cout << "Tamaño: " << tree.size() << endl;
        cout << "---" << endl;
    }
    
    cout << "Árbol AVL final:" << endl;
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
    
    cout << "Tamaño final: " << tree.size() << endl;
    cout << "¿Balanceado? " << (tree.isBalanced() ? "Sí" : "No") << endl;
    
    cout << "\n=== FIN DEMO ARBOL AVL ===" << endl;
}