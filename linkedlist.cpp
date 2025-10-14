#include <iostream>
#include "linkedlist.h"

void DemoLinkedList(){
    CLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);
    CLinkedList<int> lcopied(l1);
    // std::cout << x;
    std::cout << "Lista original: ";
    std::cout << l1 << endl;
    std::cout << "--> Lista copiada"<< std::endl;
    x = 99;
    l1.Insert(x, 16);
    std::cout << "Actual Lista original modificada: ";
    std::cout << l1 << endl;
    std::cout << "Actual Lista copiada: ";
    std::cout << lcopied << std::endl;

    CLinkedList<float> l2;
    float f = 3.14;
    l2.Insert(f, 10.5);
    // std::cout << l2;
    std::cout << "FIN"<<std::endl;
}