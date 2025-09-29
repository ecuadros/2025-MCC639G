#include <iostream>
#include "Linkedlist.h"

using namespace std;

int main(){
    LinkedList<int> list;
    list.insert(10, 1);
    list.insert(20, 2);
    list.insert(30, 3);

    std::cout << "List using operator<<: " << list << "\n";

    std::cout << "List using iterator: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    std::cout << "List using range-based for: ";
    for (auto& x : list) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}
