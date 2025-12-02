#include <iostream>
#include <vector>
#include <thread>
#include "concurrentlinkedlist.h"

void DemoConcurrentLinkedList(){
    
    std::cout << "\nDEMOSTRACIÓN DE LISTA ENLAZADA CONCURRENTE " << std::endl;
    CConcurrentLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);

    // copiando l1
    CConcurrentLinkedList<int> lcopied(l1);
    
    // std::cout << x;
    // copy cosntructor
    std::cout << ">>> Copy cosntructor" << std::endl;
    std::cout << "Lista original (l1): ";
    std::cout << l1 << std::endl;
    std::cout << "--> Lista copiada"<< std::endl;
    x = 6;
    l1.Insert(x, 16);
    std::cout << "Actual Lista original modificada: ";
    std::cout << l1 << std::endl;
    std::cout << "Actual Lista copiada: ";
    std::cout << lcopied << std::endl;

    // move constructor lcopied to lmoved
    CConcurrentLinkedList<int> lmoved(std::move(lcopied));
    std::cout << "\n>>> Move cosntructor" << std::endl;
    std::cout << "Lista incicial moved to lmoved: ";
    std::cout << "Lista origen: "<<lcopied << std::endl;
    std::cout << "--> Lista destino: "<<  lmoved <<std::endl;
    
    std::cout << "\n>>> Test concurrencia" << std::endl;
    std::vector<std::thread> thrds;
    CConcurrentLinkedList<int> l2;
    for (int i=0; i<1000; ++i){
        thrds.emplace_back([&l2, i](){
            int value = i*10;
            Ref ref   = i;
            l2.Insert(value, ref);
            std::cout << "Thread " << i << " inserted: " << value << std::endl;
        }
        );
        }
    
    for (auto& t : thrds){
        t.join();
    }
    
    
    // Iterator
    

    std::cout << "Iterando la lista: valor x 2"<< std::endl;
    // Gracias a begin() y end(), ahora esto funciona:
    for (int &valor : lmoved) {
        std::cout << valor*2 << ", ";
    }
    std::cout << std::endl;
    
    
    //std::cout << "Lista final" <<l2<<std::endl;
    std::cout << "FIN"<<std::endl;

}


