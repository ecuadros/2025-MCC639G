#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <fstream>
#include "linkedlist.h"
using namespace std;

void insertarElementos(CLinkedList<int>& lista, int inicio, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        int valor = inicio + i;
        lista.Insert(valor, i);
    }
}

void TestReadFunction() {
    cout << "\n--- Test 4: Read-istream ---" << endl;

    CLinkedList<int> lista;
    
    // Crear datos de prueba directamente
    int testData[] = {5, 100, 2, 200, 8, 300, 1, 400, 9, 500};
    stringstream ss;
    
    for (int i = 0; i < 10; i += 2) {
        ss << testData[i] << " " << testData[i + 1] << " ";
    }
    
    cout << "Datos: ";
    for (int i = 0; i < 10; i += 2) {
        cout << testData[i] << "(" << testData[i + 1] << ") ";
    }
    cout << endl;
    
    cout << "Stream contiene: '" << ss.str() << "'" << endl;
    cout << "Llamando a Read()..." << endl;
   
   
    lista.Read(ss);
    
    cout << "Estado del stream después de Read:" << endl;
    cout << " - good(): " << ss.good() << endl;
    cout << " - eof(): " << ss.eof() << endl;
    cout << " - fail(): " << ss.fail() << endl;
    cout << " - bad(): " << ss.bad() << endl;
    
    cout << "Lista resultante: " << lista << endl;
    cout << "Tamaño de la lista: " << lista.GetSize() << endl;

}

void SimpleConcurrencyTest() {
    std::cout  << "\n--- Test 3: Concurrencia ---" << std::endl;
    
    CLinkedList<int> lista;
    vector<thread> hilos;
    
    // Crear 3 hilos que insertan elementos
    for (int i = 0; i < 3; i++) {
        hilos.push_back(thread(insertarElementos, ref(lista), i * 10, 5));
    }
    
    // Esperar que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join();
    }
    
    // Mostrar resultados
    std::cout << "Tamaño final de la lista: " << lista.GetSize() << std::endl;
    std::cout << "Contenido de la lista: " << lista << std::endl;
}
void DemoLinkedList(){
 CLinkedList<int> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);
    x = 2;
    l1.Insert(x, 8);
    x = 9;
    l1.Insert(x, 1);
    
    std::cout << "Lista l1: " << l1 << std::endl;

    // Test 2: Constructor de copia
    std::cout << "\n--- Test 1: Constructor de copia ---" << std::endl;
    CLinkedList<int> l1_copy(l1);
    std::cout << "Lista original l1: " << l1 << std::endl;
    std::cout << "Copia l1_copy: " << l1_copy << std::endl;

    int y = 20;
    l1_copy.Insert(y, 99);
    std::cout << "Despues de insertar en copia:" << std::endl;
    std::cout << "Original l1: " << l1 << std::endl;
    std::cout << "Copia l1_copy: " << l1_copy << std::endl;

    std::cout << "\n--- Test 2: Move constructor ---" << std::endl;
    CLinkedList<int> l1_move(std::move(l1));
    std::cout << "Lista movida l1_move: " << l1_move << std::endl;
    std::cout << "Lista original l1 (deberia estar vacia): " << l1 << std::endl;
    std::cout << "l1 está vacía? " << (l1.IsEmpty() ? "SÍ" : "NO");


    // CLinkedList<float> l2;
    // float f = 3.14;
    // l2.Insert(f, 17);
    // std::cout <<" "<<l2;

}

