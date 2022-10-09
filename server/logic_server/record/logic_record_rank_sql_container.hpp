#pragma once

#include "logic_record_rank_skiplist.h"
#include "logic_record_sql_associate_container.hpp"

//value只能用指针存储,value的指针的创建和释放由容器托管
template <typename KeyT, typename TableT, typename TableValT>
class CLogicSQLRankAssociateContainer {  };

template <typename KeyT, typename TableT, typename TableValT>
class CLogicSQLRankAssociateContainer<KeyT, TableT*, TableValT>
{
public:
    typedef KeyT key_type;
    typedef CLogicRankSkipList<key_type> rank_container_type;
    typedef CLogicSQLRecordAssociateContainer<key_type, TableT*, TableValT> record_container_type;
    typedef typename record_container_type::table_mapped_type table_mapped_type;
    typedef typename record_container_type::table_value_type table_value_type;
    typedef typename rank_container_type::iterator iterator;
    typedef typename rank_container_type::const_iterator const_iterator;
    typedef typename rank_container_type::reverse_iterator reverse_iterator;
    typedef typename rank_container_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rank_container_type::size_type          size_type;

    CLogicSQLRankAssociateContainer() : m_stRankContainer(), m_stRecordContainer() {}

    explicit CLogicSQLRankAssociateContainer(unsigned int uiMaxRank) : m_stRankContainer(uiMaxRank), m_stRecordContainer() {}

    ~CLogicSQLRankAssociateContainer() = default;

    CLogicSQLRankAssociateContainer(const CLogicSQLRankAssociateContainer&) = delete;
    CLogicSQLRankAssociateContainer& operator=(const CLogicSQLRankAssociateContainer&) = delete;

public:
    /**
     * 返回值:插入后的排名
     */
    uint32_t Insert(const table_value_type& stVal)
    {
        auto result = m_stRankContainer.insert(stVal.first, stVal.second.m_ulRankScore);
        if (result.second)
        {
            m_stRecordContainer.Insert(stVal);
        }
        return result.first;
    }

    /**
     * 返回值:插入后的排名
     */
    uint32_t InsertWithoutSQL(const table_value_type& stVal)
    {
        auto result = m_stRankContainer.insert(stVal.first, stVal.second.m_ulRankScore);
        if (result.second)
        {
            m_stRecordContainer.InsertWithoutSQL(stVal);
        }
        return result.first;
    }

    /**
    * 返回值:修改后的排名
    */
    uint32_t UpdateOrInsert(const table_value_type& stVal)
    {
        auto [prevRank, currRank, kickKey] = m_stRankContainer.UpdateOrInsert(stVal.first, stVal.second.m_ulRankScore);
        if (currRank == 0)
        {
            m_stRecordContainer.Erase(stVal.first);
            return currRank;
        }

        auto stIter = m_stRecordContainer.Find(stVal.first);
        if (stIter == m_stRecordContainer.End())
        {
            m_stRecordContainer.Erase(kickKey);
            m_stRecordContainer.Insert(stVal);
        }
        else
        {
            stIter->second->SetRankScore(stVal.second.m_ulRankScore);
        }
        return currRank;
    }

    /**
     * 返回值:名次，0表示没有名次
     */
    uint32_t GetRank(const key_type & stKey) const
    {
        return m_stRankContainer.GetRank(stKey);
    }

    uint32_t GetRank(iterator stIT) const
    {
        return m_stRankContainer.GetRank(stIT);
    }

    inline iterator find(const key_type& stKey)
    {
        return m_stRankContainer.find(stKey);
    }

    inline void SetMaxRank(unsigned int uiMaxRank)
    {
        if(empty())
        {
            m_stRankContainer.SetMaxRank(uiMaxRank);
        }
    }

    inline void SetSortRule(bool bIsLess)
    {
        if(empty())
        {
            m_stRankContainer.SetSortRule(bIsLess);
        }
    }

    inline iterator FindByRank(uint32_t uiRank) { return (m_stRankContainer.FindByRank(uiRank)); }

    inline size_type size() const { return (m_stRankContainer.size()); };

    inline bool empty() const { return (m_stRankContainer.empty()); };

    inline iterator begin() { return (m_stRankContainer.begin()); };

    inline const_iterator begin() const { return (m_stRankContainer.begin()); };

    inline iterator end() { return (m_stRankContainer.end()); };

    inline const_iterator end() const { return (m_stRankContainer.end()); };

    inline uint32_t max_rank() const { return (m_stRankContainer.max_rank()); };

    inline void clear() { while (!empty()) erase(begin()); };

    iterator erase(iterator stIT)
    {
        if (stIT != end())
        {
            auto stRecord = m_stRecordContainer.Find(stIT->first);
            if (stRecord != m_stRecordContainer.End())
            {
                m_stRecordContainer.Erase(stRecord);
            }
        }

        return m_stRankContainer.erase(stIT);
    }

    size_type erase(const key_type &stKey)
    {
        erase(find(stKey));
        return size();
    }

private:
    rank_container_type m_stRankContainer;
    record_container_type m_stRecordContainer;
};