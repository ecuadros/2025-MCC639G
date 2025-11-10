#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "linkedlist.h"
#include "foreach.h"
#include "util.h"
#include "traits.h"

using namespace std;
using namespace std::chrono;

// Función para obtener timestamp actual formateado
string getCurrentTimestamp() {
    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    
    stringstream ss;
    ss << put_time(localtime(&time_t_now), "%H:%M:%S");
    ss << "." << setfill('0') << setw(3) << ms.count();
    return ss.str();
}

// Función para test de concurrencia de lista simple
void insertarElementos(CLinkedList<AscendingTrait<int>>& lista, int threadId, int inicio, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        int valor = inicio + i;
        int ref = threadId * 100 + i;

        // Crear mensaje completo antes de imprimir
        stringstream msg;
        msg << "[" << getCurrentTimestamp() << "] " 
            << "Hilo " << threadId << " INSERTANDO valor: " << valor;
        cout << msg.str() << endl;
        
        lista.Insert(valor, ref);
        
        
        stringstream msg2;
        msg2 << "[" << getCurrentTimestamp() << "] " 
             << "Hilo " << threadId << " COMPLETADO valor: " << valor;
        cout << msg2.str() << endl;
        
        this_thread::sleep_for(milliseconds(50));
    }
}

void TestConcurrencia() {
    cout << "\n=== TEST CONCURRENCIA LISTA SIMPLE ===" << endl;
    cout << "INICIO TEST: " << getCurrentTimestamp() << endl << endl;
    
    CLinkedList<AscendingTrait<int>> lista;
    vector<thread> hilos;
    
    for (int i = 0; i < 3; i++) {
        hilos.push_back(thread(insertarElementos, ref(lista), i, i * 10, 5));
    }
    
    for (auto& hilo : hilos) {
        hilo.join();
    }
    
    cout << "\nFIN INSERCIONES: " << getCurrentTimestamp() << endl;
    
    cout << "\n--- RESULTADOS FINALES ---" << endl;
    cout << "Tamaño final: " << lista.GetSize() << endl;
    cout << "Contenido: " << lista << endl;
    cout << "=== FIN TEST CONCURRENCIA ===" << endl;
}

void TestReadFunction() {
    cout << "\n=== TEST FUNCIÓN READ LISTA SIMPLE ===" << endl;
    
    cout << "\n--- Test Read desde stringstream ---" << endl;
    CLinkedList<AscendingTrait<int>> lista1;
    stringstream ss1;
    
    ss1 << "5 100 2 200 8 300 1 400 9 500";
    cout << "Datos de entrada: 5 100 2 200 8 300 1 400 9 500" << endl;
    
    lista1.Read(ss1);
    cout << "Lista después de Read: " << lista1 << endl;
    cout << "Tamaño: " << lista1.GetSize() << endl;
    
    cout << "\n--- Test Constructor de Copia ---" << endl;
    CLinkedList<AscendingTrait<int>> lista2(lista1);
    cout << "Original: " << lista1 << endl;
    cout << "Copia: " << lista2 << endl;
    
    int nuevoValor = 15;
    lista2.Insert(nuevoValor, 999);
    cout << "Después de insertar en copia:" << endl;
    cout << "Original: " << lista1 << endl;
    cout << "Copia: " << lista2 << endl;
    
    cout << "=== FIN TEST READ ===" << endl;
}

void TestDestructor() {
    cout << "\n=== TEST DESTRUCTOR LISTA SIMPLE ===" << endl;
    
    {
        CLinkedList<AscendingTrait<int>> listaLocal;
        int valores[] = {10, 20, 30, 40, 50};
        for (int i = 0; i < 5; i++) {
            listaLocal.Insert(valores[i], i * 10);
        }
        cout << "Lista local antes de salir del scope: " << listaLocal << endl;
        cout << "Tamaño: " << listaLocal.GetSize() << endl;
    }
    
    cout << "Scope terminado - destructor llamado exitosamente" << endl;
    cout << "=== FIN TEST DESTRUCTOR ===" << endl;
}

void DemoLinkedList(){
    cout << "\n=== DEMO LISTA SIMPLE ENLAZADA ===" << endl;
    
    CLinkedList<AscendingTrait<int>> l1;
    int x = 5;
    l1.Insert(x, 3);
    x = 7;
    l1.Insert(x, 14);
    x = 2;
    l1.Insert(x, 8);
    x = 9;
    l1.Insert(x, 1);
    
    cout << "Lista l1: " << l1 << endl;

    cout << "\n---Test01-> Constructor de copia ---" << endl;
    CLinkedList<AscendingTrait<int>> l1_copy(l1);
    cout << "Original: " << l1 << endl;
    cout << "Copia: " << l1_copy << endl;
    
    cout << "Tamaño original: " << l1.GetSize() << endl;
    cout << "Tamaño copia: " << l1_copy.GetSize() << endl;
    
    TestConcurrencia();
    TestReadFunction();
    TestDestructor();
}