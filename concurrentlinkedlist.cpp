#include <iostream>
#include <vector>
#include "concurrentlinkedlist.h"

void DemoConcurrentLinkedList(){
    
    std::cout << "\n=== DEMOSTRACIÓN DE LISTA ENLAZADA CONCURRENTE ===" << std::endl;
    CConcurrentLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);

    // copiando l1
    CConcurrentLinkedList<int> lcopied(l1);
    
    // std::cout << x;
    std::cout << ">>>> Copy cosntructor con concurerncia"<< std::endl;
    std::cout << "Lista original (l1): ";
    std::cout << l1 << std::endl;
    std::cout << "--> Lista copiada"<< std::endl;
    x = 99;
    l1.Insert(x, 16);
    std::cout << "Actual Lista original modificada: ";
    std::cout << l1 << std::endl;
    std::cout << "Actual Lista copiada: ";
    std::cout << lcopied << std::endl;

    

    
    std::cout << "FIN"<<std::endl;
}

// Demo adicional: prueba de condiciones de carrera sin mutex (para comparación)
