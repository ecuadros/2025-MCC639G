#include <iostream>
#include <string>
#include "clasestemplate.h"

using namespace std;

void DemoClasesTemplate(){
    cout << "\n=== DEMO CLASES TEMPLATE ===" << endl;
    
    CTest<int> obj1(5);
    cout << "CTest<int>: ";
    obj1.Print();

    CTest<float> obj2(3.14);
    cout << "CTest<float>: ";
    obj2.Print();

    CTest<string> obj3("Hola");
    cout << "CTest<string>: ";
    obj3.Print();

    // Ejemplo adicional con la lista doblemente enlazada
    cout << "\n--- Integración con lista doble ---" << endl;
    CTest<std::string> mensaje("Clase template funcionando!");
    cout << "Mensaje: ";
    mensaje.Print();
    
    cout << "=== FIN DEMO CLASES TEMPLATE ===" << endl;
}