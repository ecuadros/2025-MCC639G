#include <iostream>
#include <mutex>
#include <memory>
#include <fstream>
#include "binarytree.h"
#include "avl.h"
#include <vector>
#include "types.h"

#include "foreach.h"

void DemoAVL(){
    std::cout<<"--- AVL DEMO ---"<< std::endl;
    // values for AVL tree
    CAVLTree<AVLAscTraits<T1, Ref>> tree;
    
    // Insert values
    std::vector<std::pair<T1, Ref>> values = {
        {10, 1}, {20, 3}, {5, 6}, {15, 10}, {25, 11}, {3, 31}, {7,2}
    };
    
    std::cout<< "Insertando valores..."<<std::endl;
    for (const auto& par : values) {
        tree.insert(par.first, par.second);
    }
    
    std::cout << "TRee AVL: inorder print";
    std::cout << tree<< std::endl;
    std::cout << "TRee AVL: For";
    // for(auto it = tree.begin(); it != tree.end(); ++it) {
    //      cout << *it << " "; 
    // }
    
    // std::cout << "Tree contents Inorder:" << std::endl;
    // tree.inorder(std::cout);
    // std::cout << std::endl;

    // std::cout << "Tree contents Preorder:" << std::endl;
    // tree.preorder(std::cout);
    // std::cout << std::endl;

    // std::cout << "Tree contents Postorder:" << std::endl;
    // tree.postorder(std::cout);
    // std::cout << std::endl;
}