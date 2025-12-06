#include "avl.h"
#include "binarytree.h"
#include "btree.h"
#include "doublelinkedlist.h"
#include "foreach.h"
#include "linkedlist.h"
#include "types.h"
#include "util.h"
#include <fstream>
#include <iostream>
#include <utility> // para std::pair
#include <vector>

void opex(int &n) { n++; }

void DemoLinkedList() {
  std::vector<std::pair<T1, Ref>> v1 = {{4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}};
  CLinkedList<AscendingTrait<T1>> l1;
  for (auto &par : v1)
    l1.Insert(par.first, par.second);
  std::cout << l1 << std::endl;

  std::vector<std::pair<T2, Ref>> v2 = {
      {4.5, 8}, {2.3, 5}, {7.8, 3}, {1.1, 9}, {5.7, 2}};
  CLinkedList<DescendingTrait<T2>> l2;
  // Insertar desde el vector
  for (auto &par : v2) {
    l2.Insert(par.first, par.second);
  }
  std::cout << l2 << std::endl;

  std::cout << "Ahora utilizando foreach #1..." << std::endl;
  foreach (l1, ::opex)
    ;
  std::cout << "Imprimiendo: l1 (debe haber aumentado en 1) ..." << std::endl;
  foreach (l1, ::Print<T1>)
    ;
  cout << endl;
  std::cout << "l1 aplicando funcion lambda +2 ..." << std::endl;
  foreach (l1, [](T1 &n) { n += 2; })
    ;
  std::cout << "Imprimiendo: l1 (debe haber aumentado en 1) ..." << std::endl;
  foreach (l1, ::Print<T1>)
    ;

  std::cout << "Imprimiendo: l1 a través de begin() y end() ..." << std::endl;
  foreach (l1.begin(), l1.end(), ::Print<T1>)
    ;
  cout << endl;

  std::cout << "Imprimiendo l1 con Write ..." << std::endl;
  l1.Write(cout);
  cout << endl;

  std::ofstream of("LL.txt");
  l1.Write(of);
  of.close();
}

void DemoDoubleLinkedList() {
  std::vector<std::pair<T1, Ref>> v1 = {{4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}};

  std::cout << "Demo Double Linked List" << std::endl;

  CDoubleLinkedList<AscendingTrait<T1>> l1;
  for (auto &par : v1)
    l1.Insert(par.first, par.second);
  std::cout << l1 << std::endl;

  std::cout << "Imprimiendo con forward iterator" << std::endl;
  foreach (l1.begin(), l1.end(), ::Print<T1>)
    ;
  std::cout << std::endl;

  std::cout << "Imprimiendo con backward iterator" << std::endl;
  foreach (l1.rbegin(), l1.rend(), ::Print<T1>)
    ;
}

void DemoBinaryTree() {
  std::vector<std::pair<T1, Ref>> v1 = {{4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}};
  CBinaryTree<BinaryTreeAscTraits<T1>> t1;
  for (auto &par : v1)
    t1.insert(par.first, par.second);
  std::cout << t1 << std::endl;

  std::cout << "Inorder traversal:" << std::endl;
  // t1.inorder(t1.getRoot(), std::cout, 0);
  std::cout << std::endl;

  std::cout << "Preorder traversal:" << std::endl;
  // t1.preorder(t1.getRoot(), std::cout, 0);
  std::cout << std::endl;

  std::cout << "Postorder traversal:" << std::endl;
  // t1.postorder(t1.getRoot(), std::cout, 0);
  std::cout << std::endl;

  std::cout << "Tree structure:" << std::endl;
  // t1.print(t1.getRoot(), std::cout, 0);
  std::cout << std::endl;

  std::cout << "Inorder traversal using foreach:" << std::endl;
  // t1.inorder(t1.getRoot(), ::Print<T1>);
  std::cout << std::endl;

  // Next classes: AVL, BTree
}

void DemoAVLTree() {
  std::cout << "\n========== DEMO AVL TREE ==========" << std::endl;

  // Crear árbol AVL ascendente
  CAVLTree<AVLAscTraits<T1>> avl;

  // Insertar elementos (el árbol se balancea automáticamente)
  // Secuencia que causaría desbalance en un BST normal: 30, 20, 10 causa LL
  std::vector<std::pair<T1, Ref>> v1 = {{30, 1}, {20, 2}, {10, 3}, {25, 4},
                                        {40, 5}, {35, 6}, {50, 7}};

  std::cout << "Insertando elementos: ";
  for (auto &par : v1) {
    std::cout << par.first << " ";
    avl.insert(par.first, par.second);
  }
  std::cout << std::endl;

  // Mostrar usando operator<< (hereda inorder_print de CBinaryTree)
  std::cout << avl << std::endl;

  // Escribir a archivo usando Write (heredado de CBinaryTree)
  std::cout << "\nEscribiendo a archivo AVL.txt..." << std::endl;
  std::ofstream of("AVL.txt");
  avl.Write(of);
  of.close();

  // Leer el archivo para verificar
  std::cout << "Contenido de AVL.txt: ";
  std::ifstream inf("AVL.txt");
  std::string content;
  while (inf >> content) {
    std::cout << content << " ";
  }
  inf.close();
  std::cout << std::endl;

  std::cout << "========== FIN DEMO AVL ==========\n" << std::endl;
}

void DemoBTree() {
  std::cout << "\n========== DEMO B-TREE ==========" << std::endl;

  // BTree de orden 3
  BTree<BTreeTrait<int, long>> btree(3);

  // Insertar elementos
  std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 10,
                           25, 35, 45, 55, 65, 75, 85};

  std::cout << "Insertando elementos: ";
  for (size_t i = 0; i < keys.size(); i++) {
    std::cout << keys[i] << " ";
    btree.Insert(keys[i], keys[i] * 10); // ID es key * 10
  }
  std::cout << std::endl;

  // Mostrar usando operator<<
  std::cout << btree << std::endl;

  // Recorrido Inorder Variadic
  std::cout << "Inorder (Variadic):";
  btree.inorder(
      [](auto &info, size_t level, std::ostream &os) { os << " " << info.key; },
      std::cout);
  std::cout << std::endl;

  // Recorrido Preorder Variadic
  std::cout << "Preorder (Variadic):";
  btree.preorder(
      [](auto &info, size_t level, std::ostream &os) { os << " " << info.key; },
      std::cout);
  std::cout << std::endl;

  // Recorrido Postorder Variadic
  std::cout << "Postorder (Variadic):";
  btree.postorder(
      [](auto &info, size_t level, std::ostream &os) { os << " " << info.key; },
      std::cout);
  std::cout << std::endl;

  // Uso de Iteradores (foreach)
  std::cout << "Iterador forward (foreach):";
  for (auto &info : btree) {
    std::cout << " " << info.key;
  }
  std::cout << std::endl;

  // Uso de FirstThat Variadic (Buscar clave 40)
  int target = 40;
  auto *result = btree.firstThat(
      [](auto &info, size_t level, int val) { return info.key == val; },
      target);

  if (result) {
    std::cout << "FirstThat: Encontrado " << result->key << " con ID "
              << result->ObjID << std::endl;
  } else {
    std::cout << "FirstThat: No encontrado " << target << std::endl;
  }

  // Serialización (Write)
  std::cout << "Escribiendo a BTree.txt..." << std::endl;
  std::ofstream of("BTree.txt");
  btree.Write(of);
  of.close();

  // Deserialización (Read) en un nuevo árbol
  std::cout << "Leyendo de BTree.txt a un nuevo árbol..." << std::endl;
  BTree<BTreeTrait<int, long>> btree2(3);
  std::ifstream inf("BTree.txt");
  btree2.Read(inf);
  inf.close();

  std::cout << "Contenido del nuevo árbol (read):" << std::endl;
  std::cout << btree2 << std::endl;

  std::cout << "========== FIN DEMO B-TREE ==========\n" << std::endl;
}