#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

template <typename Traits>
class CAVLNode : public CBinaryTreeNode<Traits>{
public:
  using value_type = typename Traits::T;
  using Node       = CAVLNode<Traits>;
protected:
    int     m_heightNode = 0; // Altura del nodo 
   
public:
    CAVLNode(Node* pParent, KeyNode data, Ref ref, Node* p0 = nullptr, Node* p1 = nullptr)
        : CBinaryTreeNode<Traits>(pParent, data, ref, p0, p1),  m_balanceFactor(0){}//enviamos el balance

    int heightNode() {return m_heightNode;}
    //Factor de equillibrio(FE) = Altura derecha - Altura izquierda; si FE>1 o FE<-1 entonces el arbol esta desbalanceado
    //FE>1 desbalanceado a la derecha(rotar a la izquierda), FE<-1 desbalanceado a la izquierda(rotar a la derecha), FE=0 balanceado
    int balancefactor(){
        int hleft = m_pChild[0] ? m_pChild[0]->heightNode() : -1;
        int hRight = m_pChild[1] ? m_pChild[1]->heightNode() : -1;
        return hleft - hRight;
    }
    //Debe calcularse primero la altura del nodo ->h= max(hIzq, hDer)+1
    int currentNodeHeight(){
        
        int hleft = m_pChild[0] ? m_pChild[0]->heightNode() : -1;
        int hRight = m_pChild[1] ? m_pChild[1]->heightNode() : -1;
        m_heightNode = max(hleft, hRight) + 1;
    }   
        


};

template <typename _T>
struct AVLAscTraits{
    using  value_type = _T;
    using  Node       = CAVLNode<T>;
    using  CompareFn  = less<T>;
};

template <typename _T>
struct AVLDescTraits{
    using  value_type = _T;
    using  Node       = CAVLNode<T>;
    using  CompareFn  = greater<T>;
};

template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
public:
    using Base       = CBinaryTree<Traits>;
    using Node       = typename Traits::Node;
    using value_type = typename Traits::value_type;  
    using CompareFn  = typename Traits::CompareFn;
    using Container  = CAVLTree<Traits>;
    //using iterator   = binary_tree_iterator<Container>;

protected:

    Node* CreateNode(Node* pParent, value_type elem, Ref ref) {
        return new Node(pParent, elem, ref);
    }
    
    // Additional members for AVL tree balancing can be added here
    // TODO: modificar la insercion para que mantenga
    //       el balance del arbo y realice las 
    //       rotaciones necesarias
    // TODO 1. insertar
    //primero arbol avl, recorro con un insert igual al arbol binario, coloco un insert y llamo al insert del arbol binario
    Node *internal_insert(value_type &elem, Ref ref,
                          Node* pParent, Node*& rpOrigin) override
    {
        if (!rpOrigin) {
            ++m_size;
            return (rpOrigin = CreateNode(pParent, elem, ref));
        }    

        size_t branch = CompareFn(elem, rpOrigin->getDataRef()) ? 0 : 1;
        return internal_insert(elem, ref, nullptr, rpOrigin, rpOrigin->getChildRef(branch));
        rpOrigin = balance(rpOrigin);
        return rpOrigin;

    }
    // TODO 2. verificar balance
     //Factor de equillibrio(FE) = Altura derecha - Altura izquierda; si FE>1 o FE<-1 entonces el arbol esta desbalanceado
    //FE>1 desbalanceado a la derecha(rotar a la izquierda), FE<-1 desbalanceado a la izquierda(rotar a la derecha), FE=0 balanceado
    //Debe calcularse primero la altura del nodo ->h= max(hIzq, hDer)+1
    Node* balance(Node* pNode){
        if(!pNode) return nullptr;
        pNode->currentNodeHeight();
        int bf = pNode->balancefactor();

        //FE>1 rotar a la derecha, FE<-1 rotar a la izquierda, FE=0 balanceado
        if(bf > 1){
            Node* left = pNode->getChild(0);
            if (left && left->balancefactor() >= 0)
                return rightRotate(pNode);
            if (left && left->balancefactor() < 0)
            {
                pNode->getChildRef(0) = leftRotate(left);
                return rightRotate(pNode); 
            }
        }

        if(bf < -1){
            Node* right = pNode->getChild(1);
            if (right && right->balancefactor() <= 0)
            return leftRotate(pNode);
            if (right && right->balancefactor() > 0)
            {
                pNode->getChildRef(1) = rightRotate(right);
                return leftRotate(pNode); 
            }
            
        }

        return pNode;

    }
    // TODO 3. realizar rotaciones si es necesario
    //Rotación a la derecha
    Node* rightRotate(Node* ptrR){
        Node* pNodeL = ptrR->getChild(0);
        Node* pNodeR= pNodeL->getChild(1);


        pNodeL->getChildRef(1) = ptrR;
        ptrR->getChildRef(0) = pNodeR;

        if(pNodeR) pNodeR->m_pParent = ptrR;
        pNodeL->m_pParent = ptrRight->m_pParent;
        ptrR->m_pParent = pNodeL;

        ptrR->currentNodeHeight();
        pNodeL->currentNodeHeight();

        return pNodeL;

    }
    //Rotación a la izquierda
    Node* leftRotate(Node* ptrL){
        Node* pNodeR = ptrL->getChild(1);
        Node* pNodeL =  pNodeR->getChild(0);

        pNodeR->getChildRef(0) = ptrL;
        ptrL->getChildRef(1) = pNodeL;

        if(pNodeL) pNodeL->m_pParent = ptrL;
        pNodeR->m_pParent = ptrL->m_pParent;
        ptrL->m_pParent = pNodeR;

        ptrL->currentNodeHeight();
        pNodeR->currentNodeHeight();   

        return pNodeR;

    }


public:
    CAVLTree() : Base() {} // Empty tree

};

#endif // __AVL_H__