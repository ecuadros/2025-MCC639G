#include <iostream>
#include "test.h"
#include "util.h"
#include "clasestemplate.h"
#include "linkedlist.h"
#include "pointers.h"
using namespace std;

// 1. Ernesto Cuadros-Vargas
// 2. Luis Antonio Selis Vasquez
// 3. Alex Segura
// 4. Juan Felix Arias Ramos
// 5. Oscar Toledo Guerrero
// 6. Richard Villanueva Retamozo
// 7. José Bazán
// 8. Aldo Arriola
// 9. David Quispe

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
    
    DemoLinkedList();
    return 0;
}
