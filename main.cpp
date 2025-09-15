#include <iostream>
#include "test.h"

using namespace std;

// 1. Ernesto Cuadros-Vargas
// 2. Luis Antonio Selis Vasquez
// 3. Alex Segura
// 4. Juan Felix Arias Ramos
// 5. Oscar Toledo Guerrero
// 6. Richard Villanueva Retamozo
//....

// Forma 1 de Compilar: 
// g++ -std=c++17 -Wall -g -pthread -o main main.cpp test.cpp
// Forma #2 de Compilar (requiere el archivo Makefile)
// make


int main(){
    cout << "Hello EDA - MCC639G" <<endl;
    int t = prueba(5);
    cout << "Respuesta:" << t << endl;

    return 0;
}
