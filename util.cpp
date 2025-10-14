#include <iostream>
#include "util.h" 
using namespace std;

void DemoUtil(){
    int x = 5, y = 8;
    cout << "x=" << x << " y=" << y << endl;
    intercambio(x, y);
    cout << "x=" << x << " y=" << y << endl;
    
    float k = 3, i = 4;
    cout << "k=" << k << " i=" << i << endl;
    intercambio(k, i);
    cout << "k=" << k << " i=" << i << endl;
}
