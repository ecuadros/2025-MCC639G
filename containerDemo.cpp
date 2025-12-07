#include <iostream>
#include <fstream>
#include <vector>
#include <utility> // para std::pair

#include "binarytree02.h"
#include "foreach02.h"
#include "types.h"
#include "util.h"


void DemoBinaryTree(){
    std::vector< std::pair<T1, Ref> > v1 = {
        {4, 8}, {2, 5}, {7, 3}, {1, 9}, {5, 2}
    };
    CBinaryTree< AscendingTrait<T1> > bt;
    for (auto &par : v1)
        bt.insert(par.first, par.second);
    std::cout << bt << std::endl;

    std::cout << "Inorder traversal:" << std::endl;
    // bt.inorder();
    std::cout << std::endl;

    std::cout << "Preorder traversal:" << std::endl;
    // bt.preorder();
    std::cout << std::endl;

    std::cout << "Postorder traversal:" << std::endl;
    // bt.postorder();
    std::cout << std::endl;

    std::cout << "Tree structure:" << std::endl;
    // bt.print();
    std::cout << std::endl;

    // std::cout << "Imprimiendo con forward iterator" << std::endl;
    // foreach(bt. begin(), bt. end(), ::Print<T1>);
    // std::cout << std::endl;

    // std::cout << "Imprimiendo con backward iterator" << std::endl;
    // foreach(bt.rbegin(), bt.rend(), ::Print<T1>);
    // std::cout << std::endl;
    
    // std::ofstream of("BT.txt");
    // bt.Write(of);
    // of.close();

    // Next classes: AVL, BTree
}