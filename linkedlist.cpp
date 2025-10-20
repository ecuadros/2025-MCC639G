#include <iostream>
#include <fstream>
#include "linkedlist.h"

void DemoLinkedList(){
    CLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 4);

    // copiando l1
    CLinkedList<int> lcopied(l1);
    
    // std::cout << x;
    std::cout << ">>>> Copy cosntructor"<< std::endl;
    std::cout << "Lista original (l1): ";
    std::cout << l1 << std::endl;
    std::cout << "--> Lista copiada"<< std::endl;
    x = 99;
    l1.Insert(x, 16);
    std::cout << "Actual Lista original modificada: ";
    std::cout << l1 << std::endl;
    std::cout << "Actual Lista copiada: ";
    std::cout << lcopied << std::endl;

    // Read test
    CLinkedList<int> l3;
    std::ifstream datafile("datos.txt");
    std::cout << "\nRead from file datos.txt"<< std::endl;
    if (datafile.is_open()){
        l3.Read(datafile);
        datafile.close();
        std::cout << "List from file: " <<l3 << std::endl;
    }else{
        std::cout << "Cannot open fiel" << std::endl;
    }

    
    std::cout << "FIN"<<std::endl;
}