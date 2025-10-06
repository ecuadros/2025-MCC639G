
#ifndef __CLASES_TEMPLATE_H__
#define __CLASES_TEMPLATE_H__
#include <iostream>
void DemoClasesTemplate();

using namespace std;

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
    cout << m_var << endl;
}

#endif // __CLASES_TEMPLATE_H__