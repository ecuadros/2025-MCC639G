#include <iostream>
#include "test.h"
#include "power.h"
using namespace std;

// 1. Ernesto Cuadros-Vargas
// 2. Luis Antonio Selis Vasquez
// 3. Alex Segura
// 4. Juan Felix Arias Ramos
// 5. Oscar Toledo Guerrero
// 6. Aldo Arriola
// 7. Jose Bazan

// Forma 1 de Compilar: 
// g++ -std=c++17 -Wall -g -pthread -o main main.cpp test.cpp
// Forma #2 de Compilar (requiere el archivo Makefile)
// make

int main(){
    //cout << "Hello EDA - MCC639G" <<endl;
    //int t = prueba(5);
    //cout << "Respuesta:" << t << endl;

    // NUEVO: usando la función suma, Luis Selis
    //int resultado = suma(3, 7);
    float x = 5;
	float y = 3;
	float resultado = power(x, y);
	cout << "1st function:" << endl;
	cout << "5 ** 3  = power(x,y) = " << resultado << endl;

    return 0;
}
