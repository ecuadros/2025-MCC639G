#include "concurrent.h"

//
// Test de concurrencia
//
void TestConcurrency()
{
    std::cout << "\n=== TEST DE CONCURRENCIA ===\n";

    // Lista concurrente con orden ascendente de enteros
    CConcurrentLinkedList<LLinkedListAsc<int>> t2;

    // 16 hilos
    std::vector<std::thread> thrds;

    for (int i = 0; i < 16; i++) {
        thrds.emplace_back([&](){
            for (int k = 0; k < 10000; k++) {
                int v = rand() % 100000;
                int ref = v;
                t2.Insert(v, ref);
            }
        });
    }

    // Esperar a que terminen todos
    for (auto &t : thrds)
        t.join();

    std::cout << ">>> Lista final (concurrente): " << t2 << "\n";
    std::cout << "=== FIN TEST ===\n\n";
}