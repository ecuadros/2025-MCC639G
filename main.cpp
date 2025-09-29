#include <iostream>
#include <thread>
#include <vector>

#include "linked_list/LinkedList.h"
#include "double_linked_list/DoubleLinkedList.h"
#include "linked_list/MutexLinkedList.h"

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

    MutexLinkedList<int> mutex_linked_list;

    auto writer = [&mutex_linked_list](int base) {
        for (int i = 0; i < 5; i++) {
            mutex_linked_list.insert(base + i, i);
        }
    };

    auto reader = [&mutex_linked_list]() {
        std::cout << "List: " << mutex_linked_list << "\n";
    };

    std::thread t1(writer, 100);
    std::thread t2(writer, 200);
    std::thread t3(reader);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Final: " << mutex_linked_list << "\n";

    return 0;
}
