#include <iostream>
#include "test.h"

using namespace std;

void DemoTest(){
    int t = prueba(5);
    std::cout << "Respuesta:" << t << std::endl;

    // NUEVO: usando la función suma, Luis Selis
    int resultado = suma(3, 7);
    std::cout << "Suma de 3 + 7 = " << resultado << std::endl;
}

int prueba(int n){
    return n*n;
}

// Nueva función suma, Luis Selis
int suma(int a, int b){
    return a + b;
}

int division(int a, int b) {
    if (b != 0)
        return a / b;
    return 0;
}