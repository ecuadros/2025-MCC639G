#include <ostream>
#include <vector>
#include "container.h"
#include "foreach.h"
#include "util.h"

using namespace std;

void ope1(int &n){ n++; }

void DemoVector(){
    vector<int> vec = {3, 5, 7 ,1, 9, 4};

    foreach(vec, ope1);
    foreach(vec, ::Print<int>);
    cout <<endl;
    foreach(vec, [](int &n){ n+=5;  } );
    foreach(vec, ::Print<int>);
    cout <<endl;
}