#include <iostream>
//#include "test.h"
#include "util.h"
//#include "clasestemplate.h"
#include "linkedlist.h"
//#include "pointers.h"
//#include "concurrent.h"


// #include "vector.h"


using namespace std;

// Forma 1 de Compilar: 
// g++ -std=c++17 -Wall -g -pthread -o main main.cpp test.cpp
// Forma #2 de Compilar (requiere el archivo Makefile)
// make

int main(){
    cout << "Hello EDA - MCC639G" <<endl;

    // DemoTest();
    // DemoUtil();
    // DemoClasesTemplate();
    // DemoBasicPointers();

    // DemoVector();
    
    DemoLinkedList();
    //TestConcurrency();

 
    return 0;
}
