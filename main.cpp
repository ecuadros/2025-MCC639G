#include <iostream>
#include "binarytree.h"
#include "avl.h"

using namespace std;

int main() {
    cout << "=== INICIO DEMOSTRACION CONTAINERS ===" << endl;
    
    // Demo árbol binario normal
    DemoBinaryTree();
    
    // Demo árbol AVL
    DemoAVLTree();
    
    cout << "\n=== FIN DEMOSTRACION CONTAINERS ===" << endl;
    return 0;
}