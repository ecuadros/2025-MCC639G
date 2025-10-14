#include <iostream>
#include "linkedlist.h"

void DemoLinkedList(){
    CLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);
    // l1 += 5;
    std::cout << x;
    std::cout <<" "<< l1<<" "<< x <<" "<< "hola";

    CLinkedList<float> l2;
    float f = 3.14;
    l2.Insert(f, 17);
    std::cout <<" "<<l2;
}