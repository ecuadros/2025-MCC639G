#include "linkedlist.h"
#include "types.h"
#include <iostream>

void DemoLinkedList()
{
    using Traits = LLinkedListAsc<int>;
    CLinkedList<Traits> list;

    int a = 5;
    int b = 2;
    int c = 8;
    int d = 1;

    list.Insert(a, 10);
    list.Insert(b, 20);
    list.Insert(c, 30);
    list.Insert(d, 40);

    std::cout << "Contenido de la lista: " << list << "\n";

    std::cout << "Elemento en índice 2 = " << list.ReadAt(2) << "\n";

    list.WriteAt(1, 999);
    std::cout << "Lista tras WriteAt: " << list << "\n";

    std::cout << "Recorriendo con iterador: ";
    for (auto &elem : list)
        std::cout << elem << " ";
    std::cout << "\n";
}