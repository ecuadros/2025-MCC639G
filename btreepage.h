#ifndef __CBTREEPAGE_H__
#define __CBTREEPAGE_H__

#include <vector>
#include <cstddef>

// Forward declaration
template<typename Trait>
class BTree;

template<typename Trait>
class CBTreePage
{
public:
    using keyType   = typename Trait::keyType;
    using ObjIDType = typename Trait::ObjIDType;

    struct ObjectInfo
    {
        keyType     key{};
        ObjIDType   ObjID{};
        std::size_t UseCounter{0};

        ObjectInfo() = default;
        ObjectInfo(const keyType& k, const ObjIDType& v)
            : key(k), ObjID(v) {}

        operator keyType() const { return key; }
    };

    using Node = CBTreePage<Trait>;

    explicit CBTreePage(int t)
        : m_t(t)
        , m_leaf(true)
    {}

    // No copia para evitar duplicar subárboles
    CBTreePage(const CBTreePage&)            = delete;
    CBTreePage& operator=(const CBTreePage&) = delete;

    ~CBTreePage()
    {
        for (auto* c : m_children)
            delete c;
    }

private:
    int  m_t;        // grado mínimo (t)
    bool m_leaf;     // es hoja o no

    std::vector<ObjectInfo> m_keys;      // hasta 2*t-1 claves
    std::vector<Node*>      m_children;  // hasta 2*t hijos

    // Declaramos BTree completo como friend (esto da acceso a todas sus clases anidadas)
    template<typename> friend class BTree;

    bool isLeaf() const { return m_leaf; }
    bool isFull() const { return m_keys.size() >= static_cast<std::size_t>(2 * m_t - 1); }

    // ----------------------------------------------------------
    //  Insert en nodo no lleno (algoritmo clásico de B-Tree)
    // ----------------------------------------------------------
    void InsertNonFull(const keyType& k, const ObjIDType& v)
    {
        int i = static_cast<int>(m_keys.size()) - 1;

        if (m_leaf)
        {
            // hacemos espacio
            m_keys.emplace_back();  // agrega un ObjectInfo vacío al final

            while (i >= 0 && k < m_keys[static_cast<std::size_t>(i)].key)
            {
                m_keys[static_cast<std::size_t>(i + 1)] =
                    m_keys[static_cast<std::size_t>(i)];
                --i;
            }

            m_keys[static_cast<std::size_t>(i + 1)] = ObjectInfo(k, v);
        }
        else
        {
            // buscamos el hijo por donde bajar
            while (i >= 0 && k < m_keys[static_cast<std::size_t>(i)].key)
                --i;

            std::size_t childIndex = static_cast<std::size_t>(i + 1);

            // si el hijo está lleno, lo partimos
            if (m_children[childIndex]->isFull())
            {
                SplitChild(childIndex);

                // decidir si insertar en el hijo izquierdo o derecho
                if (k > m_keys[childIndex].key)
                    ++childIndex;
            }

            m_children[childIndex]->InsertNonFull(k, v);
        }
    }

    // ----------------------------------------------------------
    //  SplitChild: this = padre, dividimos hijo i
    // ----------------------------------------------------------
    void SplitChild(std::size_t i)
    {
        Node* y = m_children[i];            // hijo lleno
        Node* z = new Node(m_t);            // nuevo hijo derecho
        z->m_leaf = y->m_leaf;

        const std::size_t t = static_cast<std::size_t>(m_t);

        // clave media que sube al padre
        ObjectInfo mid = y->m_keys[t - 1];

        // z se queda con las últimas t-1 claves de y
        z->m_keys.assign(y->m_keys.begin() + static_cast<std::ptrdiff_t>(t),
                         y->m_keys.end());

        // si no es hoja, también movemos los hijos
        if (!y->m_leaf)
        {
            z->m_children.assign(y->m_children.begin() + static_cast<std::ptrdiff_t>(t),
                                 y->m_children.end());
        }

        // y se queda con las primeras t-1 claves (y t hijos si no es hoja)
        y->m_keys.resize(t - 1);
        if (!y->m_leaf)
            y->m_children.resize(t);

        // insertamos el nuevo hijo z en la lista de hijos del padre
        m_children.insert(m_children.begin() + static_cast<std::ptrdiff_t>(i + 1), z);

        // y subimos mid al vector de claves del padre
        m_keys.insert(m_keys.begin() + static_cast<std::ptrdiff_t>(i), mid);
    }

    // ----------------------------------------------------------
    //  Search en este subárbol
    // ----------------------------------------------------------
    bool Search(const keyType& key, ObjIDType& outVal) const
    {
        std::size_t i = 0;
        while (i < m_keys.size() && key > m_keys[i].key)
            ++i;

        if (i < m_keys.size() && key == m_keys[i].key)
        {
            outVal = m_keys[i].ObjID;
            const_cast<ObjectInfo&>(m_keys[i]).UseCounter++;
            return true;
        }

        if (m_leaf)
            return false;

        return m_children[i]->Search(key, outVal);
    }

    // ----------------------------------------------------------
    //  ForEach general: fn(const ObjectInfo&, size_t level)
    // ----------------------------------------------------------
public:
    template<typename Fn>
    void ForEach(std::size_t level, Fn&& fn) const
    {
        if (m_leaf)
        {
            for (const auto& info : m_keys)
                fn(info, level);
        }
        else
        {
            for (std::size_t i = 0; i < m_keys.size(); ++i)
            {
                m_children[i]->ForEach(level + 1, fn);
                fn(m_keys[i], level);
            }
            m_children[m_keys.size()]->ForEach(level + 1, fn);
        }
    }

    // ----------------------------------------------------------
    //  FirstThat general: pred(ObjectInfo&, size_t level) -> bool
    // ----------------------------------------------------------
    template<typename Pred>
    ObjectInfo* FirstThat(std::size_t level, Pred&& pred)
    {
        if (m_leaf)
        {
            for (auto& info : m_keys)
            {
                if (pred(info, level))
                    return &info;
            }
            return nullptr;
        }
        else
        {
            for (std::size_t i = 0; i < m_keys.size(); ++i)
            {
                if (auto* p = m_children[i]->FirstThat(level + 1, pred))
                    return p;

                if (pred(m_keys[i], level))
                    return &m_keys[i];
            }

            return m_children[m_keys.size()]->FirstThat(level + 1, pred);
        }
    }
};

#endif // __CBTREEPAGE_H__
