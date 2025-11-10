#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include "doublelinkedlist.h"
#include "foreach.h"
#include "util.h"

using namespace std;

// Función para test de concurrencia
void insertarElementosDLL(CDoubleLinkedList<AscendingTrait<int>>& lista, int threadId, int inicio, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        int valor = inicio + i;
        int ref = threadId * 100 + i;
        lista.Insert(valor, ref);
        this_thread::sleep_for(chrono::microseconds(10));
    }
}

void TestDoubleLinkedListConcurrency() {
    cout << "\n=== TEST CONCURRENCIA LISTA DOBLE ===" << endl;
    
    CDoubleLinkedList<AscendingTrait<int>> lista;
    vector<thread> hilos;
    
    // Crear 3 hilos que insertan elementos
    for (int i = 0; i < 3; i++) {
        hilos.push_back(thread(insertarElementosDLL, ref(lista), i, i * 10, 5));
    }
    
    // Esperar que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join();
    }
    
    cout << "Tamaño final: " << lista.Size() << endl;
       
    cout << "Contenido (forward): ";
    foreach(lista.begin(), lista.end(), ::Print<int>);
    cout << endl;
    
    cout << "Contenido (backward): ";
    
    // Test manual del backward iterator
    auto it = lista.rbegin();
    auto end = lista.rend();
    
    vector<int> backwardElements;
    while (it != end) {
        backwardElements.push_back(*it);
        ++it;
    }
    
    // Imprimir en orden inverso para verificar
    for (size_t i = 0; i < backwardElements.size(); ++i) {
        cout << backwardElements[i];
        if (i < backwardElements.size() - 1) {
            cout << " ";
        }
    }
    cout << endl;
    
    cout << "Elementos en backward: " << backwardElements.size() << endl;
    cout << "=== FIN TEST CONCURRENCIA ===" << endl;
}

void TestDoubleLinkedListRead() {
    cout << "\n=== TEST READ LISTA DOBLE ===" << endl;
    
    // Test 1: Leer desde stringstream
    cout << "\n--- Test Read desde stringstream ---" << endl;
    CDoubleLinkedList<AscendingTrait<int>> lista1;
    stringstream ss1;
    
    ss1 << "5 100 2 200 8 300 1 400 9 500";
    cout << "Datos de entrada: 5 100 2 200 8 300 1 400 9 500" << endl;
    
    lista1.Read(ss1);
    cout << "Lista después de Read: " << lista1 << endl;
    cout << "Tamaño: " << lista1.Size() << endl;
    
    // Test 2: Constructor de copia
    cout << "\n--- Test Constructor de Copia ---" << endl;
    CDoubleLinkedList<AscendingTrait<int>> lista2(lista1);
    cout << "Original: " << lista1 << endl;
    cout << "Copia: " << lista2 << endl;
    
    // Verificar que son independientes
    int nuevoValor = 15;
    lista2.Insert(nuevoValor, 999);
    cout << "Después de insertar en copia:" << endl;
    cout << "Original: " << lista1 << endl;
    cout << "Copia: " << lista2 << endl;
    
    // Test 3: Move constructor
    cout << "\n--- Test Move Constructor ---" << endl;
    CDoubleLinkedList<AscendingTrait<int>> lista3(move(lista1));
    cout << "Lista movida: " << lista3 << endl;
    cout << "Lista original después del move: " << lista1 << endl;
    cout << "¿Original vacía? " << (lista1.IsEmpty() ? "Sí" : "No") << endl;
    cout << "Tamaño original: " << lista1.Size() << endl;
    cout << "Tamaño movida: " << lista3.Size() << endl;
    
    cout << "=== FIN TEST READ ===" << endl;
}

void TestDoubleLinkedListDestructor() {
    cout << "\n=== TEST DESTRUCTOR ===" << endl;
    
    {
        CDoubleLinkedList<AscendingTrait<int>> listaLocal;
        int valores[] = {10, 20, 30, 40, 50};
        for (int i = 0; i < 5; i++) {
            listaLocal.Insert(valores[i], i * 10);
        }
        cout << "Lista local antes de salir del scope: " << listaLocal << endl;
        cout << "Tamaño: " << listaLocal.Size() << endl;
    }
    
    cout << "Scope terminado - destructor llamado exitosamente" << endl;
    cout << "=== FIN TEST DESTRUCTOR ===" << endl;
}

void DemoDoubleLinkedList(){
    vector< pair<int, Ref> > v1 = {
        {4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}
    };

    cout << "\n=== DEMO LISTA DOBLEMENTE ENLAZADA ===" << endl;

    CDoubleLinkedList<AscendingTrait<int>> l1;
    for (auto &par : v1)
        l1.Insert(par.first, par.second);
    cout << "Lista completa: " << l1 << endl;

    cout << "\nImprimiendo con forward iterator: ";
    foreach(l1.begin(), l1.end(), ::Print<int>);
    cout << endl;

    cout << "Imprimiendo con backward iterator: ";
    foreach(l1.rbegin(), l1.rend(), ::Print<int>);
    cout << endl;
    
    cout << "Tamaño de la lista: " << l1.Size() << endl;
    
    TestDoubleLinkedListConcurrency();
    TestDoubleLinkedListRead();
    TestDoubleLinkedListDestructor();
}