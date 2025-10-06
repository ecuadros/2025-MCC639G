#include <iostream>
#include <vector>
#include <utility> // para std::pair
#include <thread>        // concurrencia
#include <chrono>        // concurrencia
#include <random>        // concurrencia
#include "linkedlist.h"
#include "foreach.h"
#include "types.h"
#include "util.h"

using namespace std;
void opex(int &n){ n++; }

void DemoLinkedList(){
    std::vector< std::pair<T1, Ref> > v1 = {
        {4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}
    };
    CLinkedList< LLinkedListAsc<T1> > l1;
    for (auto &par : v1){
        int temp = par.first; //variable temporal
        //l1.Insert(par.first, par.second);
        l1.Insert(temp, par.second);
    }
        
    std::cout << l1 << std::endl;

    std::vector< std::pair<T2, Ref> > v2 = {
        {4.5, 8}, {2.3, 5}, {7.8, 3}, {1.1, 9}, {5.7, 2}
    };
    CLinkedList< LLinkedListDesc<T2> > l2;
    // Insertar desde el vector
    for (auto &par : v2) {
        float temp = par.first; //temporal
        //l2.Insert(par.first, par.second);
        l2.Insert(temp, par.second);
    }
    std::cout << l2 << std::endl;

    std::cout << "Ahora utilizando foreach #1..." << std::endl;
    foreach(l1, ::opex);
    std::cout << "Imprimiendo: l1 (debe haber aumentado en 1) ..." << std::endl;
    foreach(l1, ::Print<T1>);
    cout <<endl;
    std::cout << "l1 aplicando funcion lambda +2 ..." << std::endl;
    foreach(l1, [](T1 &n){ n += 2;  } );
    std::cout << "Imprimiendo: l1 (debe haber aumentado en 1) ..." << std::endl;
    foreach(l1, ::Print<T1>);

    std::cout << "Imprimiendo: l1 a través de begin() y end() ..." << std::endl;
    foreach(l1.begin(), l1.end(), ::Print<T1>);
    cout <<endl;
}
void DemoConcurrencia() {
    cout << "\n=== DEMO CONTROL DE CONCURRENCIA (Salida Sincronizada) ===" << endl;
    
    CLinkedList<LLinkedListAsc<int>> listaConcurrente;
    
    // Insertar algunos datos iniciales
    cout << "Insertando datos iniciales..." << endl;
    for (int i = 1; i <= 5; ++i) {
        int temp = i * 10;
        listaConcurrente.InsertThreadSafe(temp, i * 100);
    }
    
    cout << "Contenido inicial: ";
    listaConcurrente.PrintThreadSafe();
    
    vector<thread> hilos;
    
    // Función helper para salida sincronizada
    auto print_sync = [](const string& msg) {
        static std::mutex mtx;  // static interno, no se captura
        std::lock_guard<std::mutex> lock(mtx);
        cout << msg << endl;
    };
    
    // Hilo escritor
    auto escritor = [&listaConcurrente, &print_sync](int id) {
        for (int i = 0; i < 3; ++i) {
            int temp = 100 + i;
            listaConcurrente.InsertThreadSafe(temp, temp * 10);
            print_sync("Hilo escritor " + to_string(id) + " insertó: " + to_string(temp));
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    };
    
    // Hilo lector
    auto lector = [&listaConcurrente, &print_sync](int id) {
        for (int i = 0; i < 3; ++i) {
            size_t tamano = listaConcurrente.SizeThreadSafe();
            print_sync("Hilo lector " + to_string(id) + " - Tamaño actual: " + to_string(tamano));
            this_thread::sleep_for(chrono::milliseconds(150));
        }
    };
    
    // Hilo buscador
    auto buscador = [&listaConcurrente, &print_sync](int id) {
        vector<int> valores = {10, 50, 100, 200};
        for (int valor : valores) {
            bool encontrado = listaConcurrente.FindThreadSafe(valor);
            print_sync("Hilo buscador " + to_string(id) + " - Valor " + to_string(valor) + 
                      ": " + (encontrado ? "ENCONTRADO" : "NO encontrado"));
            this_thread::sleep_for(chrono::milliseconds(80));
        }
    };
    
    // Crear hilos
    hilos.emplace_back(escritor, 1);
    hilos.emplace_back(lector, 1);
    hilos.emplace_back(buscador, 1);
    hilos.emplace_back(lector, 2);
    
    for (auto &hilo : hilos) {
        hilo.join();
    }
    
    cout << "\n=== RESULTADO FINAL ===" << endl;
    cout << "Contenido final: ";
    listaConcurrente.PrintThreadSafe();
    
    cout << "\n=== OPERACIONES CON LAMBDA ===" << endl;
    
    // Operación de escritura
    listaConcurrente.WriteLockedOperation([&]() {
        cout << "Operación de escritura - Insertando elemento especial 999..." << endl;
        int temp = 999;
        listaConcurrente.Insert(temp, 9999);
    });
    
    // Operación de lectura
    listaConcurrente.ReadLockedOperation([&]() {
        cout << "Operación de lectura - Recorriendo lista..." << endl;
        auto current = listaConcurrente.GetRoot();
        int count = 0;
        while (current) {
            ++count;
            cout << current->GetData() << " ";
            current = current->GetNext();
        }
        cout << "\nTotal de elementos: " << count << endl;
    });
    
    cout << "Demo de concurrencia completada exitosamente!" << endl;
}