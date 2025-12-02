#include <iostream>
#include "clasestemplate.h"
using namespace std;

void DemoClasesTemplate(){
    cout << "Demo de clases:\n";
    CTest<int> obj1(5);
    obj1.Print();

    CTest<float> obj2(3.14);
    obj2.Print();

    CTest<string> obj3("Hola");
    obj3.Print();
}