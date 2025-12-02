#ifndef __CLASES_TEMPLATE_H__
#define __CLASES_TEMPLATE_H__

#include <iostream>
#include <string>

// Eliminar using namespace std; de headers
template <typename T>
class CTest{
private:
    T m_var;
public:
    CTest(T var);
    void Print();
};

template <typename T>
CTest<T>::CTest(T var){
    m_var = var;
}

template <typename T>
void CTest<T>::Print(){
    std::cout << m_var << std::endl;
}

// Declaración de la función demo
void DemoClasesTemplate();

#endif // __CLASES_TEMPLATE_H__