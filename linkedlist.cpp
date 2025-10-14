#include <iostream>
#include "linkedlist.h"

void DemoLinkedList(){
 CLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);
    x = 2;
    l1.Insert(x, 8);
    x = 9;
    l1.Insert(x, 1);
    
    std::cout << "Lista l1: " << l1 << std::endl;

    // Test 2: Constructor de copia
    std::cout << "\n--- Test 2: Constructor de copia ---" << std::endl;
    CLinkedList<int> l1_copy(l1);
    std::cout << "Lista original l1: " << l1 << std::endl;
    std::cout << "Copia l1_copy: " << l1_copy << std::endl;

    int y = 20;
    l1_copy.Insert(y, 99);
    std::cout << "Despues de insertar en copia:" << std::endl;
    std::cout << "Original l1: " << l1 << std::endl;
    std::cout << "Copia l1_copy: " << l1_copy << std::endl;

    std::cout << "\n--- Test 3: Move constructor ---" << std::endl;
    CLinkedList<int> l1_move(std::move(l1));
    std::cout << "Lista movida l1_move: " << l1_move << std::endl;
    std::cout << "Lista original l1 (deberia estar vacia): " << l1 << std::endl;
    std::cout << "l1 está vacía? " << (l1.IsEmpty() ? "SÍ" : "NO");


    // CLinkedList<float> l2;
    // float f = 3.14;
    // l2.Insert(f, 17);
    // std::cout <<" "<<l2;

}