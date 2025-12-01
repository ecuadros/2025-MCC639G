#include <iostream>
#include <sstream>
#include "doublelinkedlist.h"

using std::cout;
using std::endl;

// ============================================
//  DEMOSTRACIÓN DE LA LISTA DOBLE ENLAZADA
// ============================================
void DemoDoubleLinkedList(){

    cout << "\n=====================================\n";
    cout << " DEMO CDoubleLinkedList\n";
    cout << "=====================================\n\n";

    // Definición del Traits mínimo
    struct MyTraits{
        using value_type = int;
        using Func = std::less<int>;
    };

    CDoubleLinkedList<MyTraits> lista;

    cout << "Insertando elementos...\n";
    int a=10, b=5, c=20, d=15;
    lista.Insert(a, 100);
    lista.Insert(b, 200);
    lista.Insert(c, 300);
    lista.Insert(d, 400);

    cout << "Contenido actual (forward iterator): ";
    for(auto it = lista.begin(); it != lista.end(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "Contenido actual (reverse iterator): ";
    for(auto it = lista.rbegin(); it != lista.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    // -----------------------------------------
    // COPY CONSTRUCTOR
    // -----------------------------------------
    cout << "\nProbando COPY CONSTRUCTOR...\n";
    CDoubleLinkedList<MyTraits> copia(lista);

    cout << "Lista copiada: ";
    for(auto it = copia.begin(); it != copia.end(); ++it)
        cout << *it << " ";
    cout << endl;

    // -----------------------------------------
    // MOVE CONSTRUCTOR
    // -----------------------------------------
    cout << "\nProbando MOVE CONSTRUCTOR...\n";
    CDoubleLinkedList<MyTraits> movida(std::move(copia));

    cout << "Lista movida: ";
    for(auto it = movida.begin(); it != movida.end(); ++it)
        cout << *it << " ";
    cout << endl;

    // -----------------------------------------
    // READ (istream)
    // -----------------------------------------
    cout << "\nProbando Read(istream)...\n";

    std::stringstream ss;
    ss << "7 700 8 800 1 111";

    movida.Read(ss);

    cout << "Contenido después de Read: ";
    for(auto it = movida.begin(); it != movida.end(); ++it)
        cout << *it << " ";
    cout << endl;

    // -----------------------------------------
    // WRITE
    // -----------------------------------------
    cout << "\nProbando Write(ostream)...\n";

    cout << "Resultado de Write(): ";
    movida.Write(cout);
    cout << endl;

    cout << "\nFIN DE DEMO\n\n";
}