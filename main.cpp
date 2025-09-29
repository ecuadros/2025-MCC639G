#include <iostream>
#include "LinkedList/LinkedList.h"
#include "DoubleLinkedList/DoubleLinkedList.h"

using namespace std;

int main(){
    LinkedList<int> linked_list;
    linked_list.insert(10, 1);
    linked_list.insert(20, 2);
    linked_list.insert(30, 3);

    std::cout << "List using operator<<: " << linked_list << "\n";

    std::cout << "List using iterator: ";
    for (auto it = linked_list.begin(); it != linked_list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    std::cout << "List using range-based for: ";
    for (auto& x : linked_list) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    DoubleLinkedList<int> double_linked_list;

    double_linked_list.PushBack(10, 1);
    double_linked_list.PushBack(20, 2);
    double_linked_list.PushBack(30, 3);

    std::cout << "Forward: ";
    for (auto it = double_linked_list.begin(); it != double_linked_list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    std::cout << "Backward: ";
    for (auto it = double_linked_list.rbegin(); it != double_linked_list.rend(); --it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    return 0;
}
