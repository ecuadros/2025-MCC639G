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
    for(auto it = tree.begin(); it != tree.end(); ++it) {
         cout << *it << " "; 
    }
    
    std::cout << "\nTree contents Inorder:" << std::endl;
    tree.inorder(std::cout);
    std::cout << std::endl;

    std::cout << "\nTree contents Preorder:" << std::endl;
    tree.preorder(std::cout);
    std::cout << std::endl;

    std::cout << "\nTree contents Postorder:" << std::endl;
    tree.postorder(std::cout);
    std::cout << std::endl;
    
    // write test
    std::cout << "\nWrite Test (preorder)"<< std::endl;

    ofstream file("out_AVL_preorder_binarytree.txt");
    if (!file.is_open()) {
        cerr << "Error: Cannot open file  for writing" << std::endl;
        return ;
    }
    tree.preorder(file);
    file << std::endl;
    file.close();
    std::cout << "-> file saved"<< std::endl;
        // Reat test
    std::cout << "\nRead Test (preorder)"<< std::endl;
    
    CBinaryTree<BinaryTreeAscTraits<int>> read_tree;
    cout << "\nLoading from file: " << "out_preorder_binarytree.txt" << endl;
    if (read_tree.ReadFromFile("out_AVL_preorder_binarytree.txt")) {
        cout << "-> Read tree (Preorder):" << endl;
        read_tree.preorder(cout);

        cout << "-> Read tree (Inorder):" << endl;
        read_tree.inorder(cout);
        
        // Verificar que son iguales
        cout << "\nVerification:" << endl;
        cout << "Original size: " << tree.size() << endl;
        cout << "Loaded size: " << read_tree.size() << endl;
    }
}