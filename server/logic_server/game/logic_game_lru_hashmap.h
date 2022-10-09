#pragma once

#include <list>
#include <utility>
#include <functional>
#include <bits/c++config.h>
#include <backward/hashtable.h>
#include <bits/concept_check.h>

template <typename KeyT, typename ValT> struct logic_lru_hashmap_list_t
{
    typedef std::pair<KeyT, ValT>                   value_type;
    typedef std::list<value_type>                   list_type;
    typedef typename list_type::iterator            node_type;
    typedef typename list_type::iterator            iterator;
    typedef typename list_type::const_iterator      const_iterator;
};

//提取key的仿函数,针对rk_logic_lru_hashmap_list_t::node_type
template <typename T> struct logic_lru_hash_node_selector : public std::unary_function<T, typename T::value_type::first_type>
{
    typename T::value_type::first_type& operator()(T& stNode) const { return (stNode->first); }

    const typename T::value_type::first_type& operator()(const T& stNode) const { return (stNode->first); }
};

template<typename KeyT, typename ValT, class HashFn = __gnu_cxx::hash<KeyT>, 
         class EqualKey = std::equal_to<KeyT>, class Alloc = std::allocator<ValT> >
class CLogicLRUHashmap
{
public:
    enum DEFAULT_PARAM
    {
        DEFAULT_INIT_HASHTABLE_SIZE = 7000
    };
    
public:
    typedef typename logic_lru_hashmap_list_t<KeyT, ValT>::list_type list_type;
    typedef typename logic_lru_hashmap_list_t<KeyT, ValT>::node_type node_type;
	typedef logic_lru_hash_node_selector<node_type> lru_hash_node_selector_type;
    typedef __gnu_cxx::hashtable<node_type, KeyT, HashFn, lru_hash_node_selector_type, EqualKey, Alloc> hash_table_type;
    
    typedef typename hash_table_type::key_type key_type;
    typedef ValT data_type;
    typedef ValT mapped_type;
    typedef typename logic_lru_hashmap_list_t<KeyT, ValT>::value_type value_type;
    typedef typename hash_table_type::value_type internal_value_type;
    typedef typename hash_table_type::hasher hasher;
    typedef typename hash_table_type::key_equal key_equal;

    typedef typename hash_table_type::size_type size_type;

    typedef typename hash_table_type::iterator internal_iterator;
    typedef typename hash_table_type::const_iterator const_internal_iterator;

    typedef typename hash_table_type::allocator_type allocator_type;
    
    typedef typename logic_lru_hashmap_list_t<KeyT, ValT>::iterator iterator;
    typedef typename logic_lru_hashmap_list_t<KeyT, ValT>::const_iterator const_iterator;
    
    typedef std::function<void(const KeyT&, const ValT&)> notifier_callback_type;
    
public:
    CLogicLRUHashmap()
    : m_stLRUList(), m_stHashTable((size_type)DEFAULT_INIT_HASHTABLE_SIZE, hasher(), key_equal(), allocator_type()), 
      m_stNotifier(), m_uiMaxNodeNum((size_type)DEFAULT_INIT_HASHTABLE_SIZE) {}

    explicit CLogicLRUHashmap(size_type uiInitSize)
    : m_stLRUList(), m_stHashTable(uiInitSize, hasher(), key_equal(), allocator_type()), 
      m_stNotifier(), m_uiMaxNodeNum(uiInitSize) {}

    //O(1)
    size_type size() const { return (m_stHashTable.size()); }

    bool empty() const { return (m_stLRUList.empty()); }

    iterator begin() { return (m_stLRUList.begin()); }

    iterator end() { return (m_stLRUList.end()); }

    const_iterator begin() const { return (m_stLRUList.begin()); }

    const_iterator end() const { return (m_stLRUList.end()); }

    void SetKickOutNotifier(const notifier_callback_type& stNotifier)
    {
        m_stNotifier = stNotifier;
    }
    
    std::pair<iterator, bool> insert(const value_type& stObj)
    { 
        if(m_stHashTable.find(stObj.first) != m_stHashTable.end())
        {
            return (std::pair<iterator, bool>(m_stLRUList.end(), false));
        }
        
        if(m_stHashTable.size() >= m_uiMaxNodeNum) DelOldestElem();
        
        node_type stNode = AddNodeToNewest(stObj);
        
        m_stHashTable.insert_unique_noresize(stNode);

        return (std::pair<iterator, bool>(stNode, true)); 
    }

    iterator find(const key_type& stKey)
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if(stIter != m_stHashTable.end()) 
        {
//            UpdateElemToNewest(*stIter);
//            return (*stIter);
            
            return (UpdateElemToNewest(*stIter));
        }
        
        return (m_stLRUList.end()); 
    }

    const_iterator find(const key_type& stKey) const
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if(stIter != m_stHashTable.end()) 
        {
//            UpdateElemToNewest(*stIter);
//            return (const_iterator(*stIter));
            
            return (const_iterator(UpdateElemToNewest(*stIter)));
        }
        
        return (m_stLRUList.end()); 
    }
    
    iterator FindNoMove(const key_type& stKey)
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if(stIter != m_stHashTable.end()) return (*stIter);
        
        return (m_stLRUList.end()); 
    }
    
    const_iterator FindNoMove(const key_type& stKey) const
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if(stIter != m_stHashTable.end()) return (const_iterator(*stIter));
        
        return (m_stLRUList.end()); 
    }

    ValT& operator[](const key_type& stKey)
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if(stIter != m_stHashTable.end()) 
        {
//            UpdateElemToNewest(*stIter);
//            return (stIter->second);
            return (UpdateElemToNewest(*stIter)->second);
        }
        else
        {
            std::pair<iterator, bool> stRet = insert(value_type(stKey, ValT()));
            return (stRet->first->second);
        }
    }

    size_type erase(const key_type& stKey)
    {
        internal_iterator stInternalIter = m_stHashTable.find(stKey);
        if(stInternalIter != m_stHashTable.end())
        {
            DelElem(*stInternalIter);
            m_stHashTable.erase(stInternalIter);
        }
        
        return (m_stHashTable.size()); 
    }

    void erase(iterator stIter)
    {
        if(end() != stIter)
        {
            m_stHashTable.erase(stIter->first);
            DelElem(stIter);
        }
    }

    void clear()
    { 
        m_stHashTable.clear();
        m_stLRUList.clear();
    }

private:
    bool IsNotifierValid() { return ((bool)m_stNotifier); }
    
    void NotifyKickout(const value_type& stVal) { if(IsNotifierValid()) m_stNotifier(stVal.first, stVal.second); }
    
    node_type AddNodeToNewest(const value_type& stVal)
    {
        m_stLRUList.push_front(stVal);
        return (m_stLRUList.begin());
    }
    
    node_type UpdateElemToNewest(node_type& stNode)
    {
        value_type stValue(*stNode);
        m_stLRUList.erase(stNode);
        m_stLRUList.push_front(stValue);
        stNode = m_stLRUList.begin();
        return (stNode);
    }
    
    void DelOldestElem()
    {
        value_type stVal = m_stLRUList.back();
        m_stHashTable.erase(stVal.first);
        NotifyKickout(stVal);
        m_stLRUList.pop_back();
    }
    
    void DelElem(node_type& stNode) { m_stLRUList.erase(stNode); }
    
private:
    list_type                       m_stLRUList;
    mutable hash_table_type         m_stHashTable;
    notifier_callback_type          m_stNotifier;
    
    size_type                       m_uiMaxNodeNum;
};
