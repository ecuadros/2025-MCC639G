

#include <iostream>
#include <mutex>
#include <memory>
#include <fstream>
#include "types.h"
#include "binarytree.h"


void DemoBinaryTree() {
    
    std::cout << "Binary Tree Implementation" <<std::endl;
    CBinaryTree<BinaryTreeAscTraits<T1>> tree;
    std::vector< std::pair<T1, Ref> > v1 = {
        {7, 8}, {5, 9}, {6, 3}, {2, 9}, {10, 2}, {8, 2}
    };
    
    std::cout << "Input values:"<< std::endl;
    for(auto pr: v1)
    {
      std::cout << "{" << pr.first << ", " <<  pr.second << "}  ";
    }
    std::cout << "\n\n";
    for (auto &par : v1)
        tree.insert(par.first, par.second);
    //std::cout << tree << std::endl;

    
    std::cout << "Tree contents Inorder:" << std::endl;
    tree.inorder(std::cout);
    std::cout << std::endl;

    std::cout << "Tree contents Preorder:" << std::endl;
    tree.preorder(std::cout);
    std::cout << std::endl;

    std::cout << "Tree contents Postorder:" << std::endl;
    tree.postorder(std::cout);
    std::cout << std::endl;
    
    std::cout << "Tree structure:" << std::endl;
    tree.print(cout);

    std::cout << "Write Test (preorder)"<< std::endl;

    ofstream file("out_preorder_binarytree.txt");
    if (!file.is_open()) {
        cerr << "Error: Cannot open file  for writing" << std::endl;
        return ;
    }
    tree.preorder(file);
    file << std::endl;
    file.close();
    std::cout << "-> file saved"<< std::endl;

    // Reat test
    std::cout << "Read Test (preorder)"<< std::endl;
    
    CBinaryTree<BinaryTreeAscTraits<int>> read_tree;
    cout << "\nLoading from file: " << "out_preorder_binarytree.txt" << endl;
    if (read_tree.ReadFromFile("out_preorder_binarytree.txt")) {
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