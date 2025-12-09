#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include "btreepage.h"

// Trait generico
template<typename _keyType, typename _ObjIDType>
struct BTreeTrait
{
    using keyType        = _keyType;
    using ObjIDType      = _ObjIDType;
    using LinkedValueType= _ObjIDType;
};

template<typename Trait>
class BTree
{
public:
    using keyType    = typename Trait::keyType;
    using ObjIDType  = typename Trait::ObjIDType;
    using Node       = CBTreePage<Trait>;
    using ObjectInfo = typename Node::ObjectInfo;


    // ------------------------------------------------------
    //  ForEach general (thread-safe)
    // ------------------------------------------------------
    template<typename Fn>
    void ForEach(Fn&& fn) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        if (!m_root) return;
        m_root->ForEach(0, std::forward<Fn>(fn));
    }

    // ------------------------------------------------------
    //  FirstThat general (thread-safe)
    // ------------------------------------------------------
    template<typename Pred>
    ObjectInfo* FirstThat(Pred&& pred)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        if (!m_root) return nullptr;
        return m_root->FirstThat(0, std::forward<Pred>(pred));
    }
    
    
    // ------------------------------------------------------
    //  Constructores / destructor
    // ------------------------------------------------------
    BTree(int order = 3, bool unique = true)
        : m_t(order)
        , m_root(new Node(order))
        , m_unique(unique)
        , m_numKeys(0)
    {}

    ~BTree()
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        delete m_root;
    }

    // No copiamos para simplificar
    BTree(const BTree&)            = delete;
    BTree& operator=(const BTree&) = delete;


    // ------------------------------------------------------
    //  Forward Iterator
    // ------------------------------------------------------
    class forward_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ObjectInfo;
        using difference_type   = std::ptrdiff_t;
        using pointer           = ObjectInfo*;
        using reference         = ObjectInfo&;

        forward_iterator() : m_current(nullptr), m_index(0) {}
        
        forward_iterator(const BTree* tree) : m_current(nullptr), m_index(0)
        {
            if (tree && tree->m_root)
            {
                collectInOrder(tree->m_root);
                if (!m_items.empty())
                    m_current = m_items[0];
            }
        }

        reference operator*() const { return *m_current; }
        pointer operator->() const { return m_current; }

        forward_iterator& operator++()
        {
            if (m_index + 1 < m_items.size())
            {
                ++m_index;
                m_current = m_items[m_index];
            }
            else
            {
                m_current = nullptr;
            }
            return *this;
        }

        forward_iterator operator++(int)
        {
            forward_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const forward_iterator& other) const
        {
            return m_current == other.m_current;
        }

        bool operator!=(const forward_iterator& other) const
        {
            return !(*this == other);
        }

    private:
        void collectInOrder(Node* node)
        {
            if (!node) return;

            if (node->m_leaf)
            {
                for (auto& info : node->m_keys)
                    m_items.push_back(&info);
            }
            else
            {
                for (std::size_t i = 0; i < node->m_keys.size(); ++i)
                {
                    collectInOrder(node->m_children[i]);
                    m_items.push_back(&(node->m_keys[i]));
                }
                collectInOrder(node->m_children[node->m_keys.size()]);
            }
        }

        ObjectInfo* m_current;
        std::size_t m_index;
        std::vector<ObjectInfo*> m_items;
    };

    // ------------------------------------------------------
    //  Backward Iterator
    // ------------------------------------------------------
    class backward_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ObjectInfo;
        using difference_type   = std::ptrdiff_t;
        using pointer           = ObjectInfo*;
        using reference         = ObjectInfo&;

        backward_iterator() : m_current(nullptr), m_index(0) {}
        
        backward_iterator(const BTree* tree) : m_current(nullptr), m_index(0)
        {
            if (tree && tree->m_root)
            {
                collectInOrder(tree->m_root);
                if (!m_items.empty())
                {
                    m_index = m_items.size() - 1;
                    m_current = m_items[m_index];
                }
            }
        }

        reference operator*() const { return *m_current; }
        pointer operator->() const { return m_current; }

        backward_iterator& operator++()
        {
            if (m_index > 0)
            {
                --m_index;
                m_current = m_items[m_index];
            }
            else
            {
                m_current = nullptr;
            }
            return *this;
        }

        backward_iterator operator++(int)
        {
            backward_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const backward_iterator& other) const
        {
            return m_current == other.m_current;
        }

        bool operator!=(const backward_iterator& other) const
        {
            return !(*this == other);
        }

    private:
        void collectInOrder(Node* node)
        {
            if (!node) return;

            if (node->m_leaf)
            {
                for (auto& info : node->m_keys)
                    m_items.push_back(&info);
            }
            else
            {
                for (std::size_t i = 0; i < node->m_keys.size(); ++i)
                {
                    collectInOrder(node->m_children[i]);
                    m_items.push_back(&(node->m_keys[i]));
                }
                collectInOrder(node->m_children[node->m_keys.size()]);
            }
        }

        ObjectInfo* m_current;
        std::size_t m_index;
        std::vector<ObjectInfo*> m_items;
    };
    
    // ------------------------------------------------------
    //  Iterator methods
    // ------------------------------------------------------
    forward_iterator begin() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return forward_iterator(this);
    }

    forward_iterator end() const
    {
        return forward_iterator();
    }

    backward_iterator rbegin() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return backward_iterator(this);
    }

    backward_iterator rend() const
    {
        return backward_iterator();
    }

    // ------------------------------------------------------
    //  Insert (thread-safe)
    // ------------------------------------------------------
    bool Insert(const keyType& k, ObjIDType v)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        // Si las claves deben ser unicas, verificamos antes
        if (m_unique)
        {
            ObjIDType tmp{};
            if (SearchInternal(k, tmp))
                return false;     // ya existe
        }

        if (!m_root)
            m_root = new Node(m_t);

        // Si la raiz esta llena, creamos nueva raiz y partimos
        if (m_root->isFull())
        {
            Node* s = new Node(m_t);
            s->m_leaf = false;
            s->m_children.push_back(m_root);
            s->SplitChild(0);
            m_root = s;
        }

        m_root->InsertNonFull(k, v);
        ++m_numKeys;
        return true;
    }

    // ------------------------------------------------------
    //  Search (thread-safe)
    // ------------------------------------------------------
    bool Search(const keyType& k, ObjIDType& outVal) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return SearchInternal(k, outVal);
    }



    // ------------------------------------------------------
    //  Print usando ForEach
    // ------------------------------------------------------
    void Print(std::ostream& os) const
    {
        ForEach([&](const ObjectInfo& info, std::size_t level)
        {
            for (std::size_t i = 0; i < level; ++i)
                os << "\t";
            os << info.key << "->" << info.ObjID << "\n";
        });
    }

    // ------------------------------------------------------
    //  size (thread-safe)
    // ------------------------------------------------------
    std::size_t size() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_numKeys;
    }

    // ------------------------------------------------------
    //  Serializacion: Write / Read (thread-safe)
    // ------------------------------------------------------
    bool Write(const std::string& filename) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) return false;

        ofs.write(reinterpret_cast<const char*>(&m_t),       sizeof(m_t));
        ofs.write(reinterpret_cast<const char*>(&m_unique),  sizeof(m_unique));
        ofs.write(reinterpret_cast<const char*>(&m_numKeys), sizeof(m_numKeys));

        ForEachInternal([&](const ObjectInfo& info, std::size_t)
        {
            ofs.write(reinterpret_cast<const char*>(&info.key),   sizeof(info.key));
            ofs.write(reinterpret_cast<const char*>(&info.ObjID), sizeof(info.ObjID));
        });

        return true;
    }

    bool Read(const std::string& filename)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) return false;

        int         order  = 3;
        bool        uniq   = true;
        std::size_t count  = 0;

        ifs.read(reinterpret_cast<char*>(&order), sizeof(order));
        ifs.read(reinterpret_cast<char*>(&uniq),  sizeof(uniq));
        ifs.read(reinterpret_cast<char*>(&count), sizeof(count));

        delete m_root;
        m_t       = order;
        m_unique  = uniq;
        m_numKeys = 0;
        m_root    = new Node(m_t);

       for (std::size_t i = 0; i < count; ++i)
       {
       keyType   key{};
       ObjIDType objId{};

       ifs.read(reinterpret_cast<char*>(&key),  sizeof(key));
       ifs.read(reinterpret_cast<char*>(&objId), sizeof(objId));

       if (m_root->isFull())
       {
              Node* s = new Node(m_t);
              s->m_leaf = false;
              s->m_children.push_back(m_root);
              s->SplitChild(0);
              m_root = s;
       }

       m_root->InsertNonFull(key, objId);
       ++m_numKeys;
       }

        return true;
    }

    // ------------------------------------------------------
    //  operator<< (thread-safe)
    // ------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const BTree& tree)
    {
        tree.Print(os);
        return os;
    }

    // ------------------------------------------------------
    //  operator>> (thread-safe)
    // ------------------------------------------------------
       friend std::istream& operator>>(std::istream& is, BTree& tree)
       {
       std::unique_lock<std::shared_mutex> lock(tree.m_mutex);
       
       keyType   key;
       ObjIDType objId;
       
       while (is >> key >> objId)
       {
              // Insertamos sin lock (ya lo tenemos)
              if (!tree.m_root)
              tree.m_root = new Node(tree.m_t);

              if (tree.m_root->isFull())
              {
              Node* s = new Node(tree.m_t);
              s->m_leaf = false;
              s->m_children.push_back(tree.m_root);
              s->SplitChild(0);
              tree.m_root = s;
              }

              tree.m_root->InsertNonFull(key, objId);
              ++tree.m_numKeys;
       }
       
       return is;
       }


private:
    int         m_t;
    Node*       m_root;
    bool        m_unique;
    std::size_t m_numKeys;
    mutable std::shared_mutex m_mutex;  // Para concurrencia

    // Métodos internos sin locks (para uso interno cuando ya tenemos el lock)
    bool SearchInternal(const keyType& key, ObjIDType& outObjId) const
    {
        if (!m_root) return false;
        return m_root->Search(key, outObjId);
    }

    template<typename Fn>
    void ForEachInternal(Fn&& fn) const
    {
        if (!m_root) return;
        m_root->ForEach(0, std::forward<Fn>(fn));
    }
};

#endif // __BTREE_H__
