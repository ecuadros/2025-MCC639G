#include <iostream>
#include <fstream>
#include <vector>
#include <utility> // para std::pair
#include "linkedlist.h"
#include "doublelinkedlist.h"
#include "foreach.h"
#include "types.h"
#include "util.h"

void add2(int &n){ n=n+2; }

void DemoDoubleLinkedList(){
    std::vector< std::pair<T1, Ref> > v1 = {
        {4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}
    };
    CDoubleLinkedList< AscendingTrait<T1> > l1;
    for (auto &par : v1)
        l1.Insert(par.first, par.second);
    std::cout << l1 << std::endl;

    foreach(l1. begin(), l1. end(), ::Print<T1>);
    foreach(l1.rbegin(), l1.rend(), ::Print<T1>);
    std::cout<< endl;

    // copy constructor test
    std::cout<< "Copy Cosntructor test" << std::endl;
    CDoubleLinkedList<AscendingTrait<T1> > lcopied(l1);
    std::cout << "Lista original (l1): ";
    std::cout << l1 << std::endl;
    std::cout << "--> Lista copiada"<< std::endl;
    // inssertng an element
    foreach(l1.begin(), l1.end(), add2);
    std::cout << "Actual Lista original modificada: ";
    std::cout << l1 << std::endl;
    std::cout << "Actual Lista copiada: ";
    std::cout << lcopied << std::endl;
    std::cout << "\nDestructor Implemented\n";
}