#include <iostream>
#include "pointers.h"
using namespace std;

template <typename T>
void Print(T &n){   cout << "val: " << n << endl; }

// int by value
void f1(int n) { n++;    }
// int by reference
void f2(int &n){ n++;    }

// passing a pointer by value
void f3(int *pi){
    ++*pi;
    pi = nullptr;
}

// passing a pointer by value
void f4(int *&pi){
    ++*pi;
    pi = nullptr;
}

void DemoBasicPointers(){
    int a = 10, b=20, &ref = a;
    int *p = nullptr, 
        *q = nullptr, 
        **pp= nullptr;

    p = &a;     q = &b;
    pp = &p;
    *p += 2;    // a += 2;

    cout << "Paso int por valor" << endl;
    f1(a);
    f1(5);
    f1(a+20);
    f1(*p);
    f1(ref);
    f1(**pp);
    f1(*q);
    Print(a);

    cout << "Paso int por referencia" << endl;
    a = 10;
    f2(a);
    // f2(a+5); error
    // f2(8);   error
    f2(*p);
    f2(**pp);
    Print(a);

    cout << "Paso de un puntero por valor" << endl;
    a = 10;
    f3(&a);   // 11
    f3(p);    // 12
    f3(*pp);  // 13
    f3(&ref); // &a
    Print(a);
    Print(p);

    cout << "Paso de un puntero por referencia" << endl;
    a = 10;
    // f4(&a);
    f4(p);
    Print(a);   // 11 pero p se convierte en nullptr
    Print(p);
    p = &a;
    f4(*pp); // f4(p); a pasa a 12 y p se convierte en nullptr
    Print(a);
    Print(p);
}