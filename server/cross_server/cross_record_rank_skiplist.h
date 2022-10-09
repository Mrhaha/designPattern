#pragma once

#include <tuple>
#include <functional>
#include <unordered_map>

#include "tc_skiplist.h"

template <class KeyT>
class CLogicRankSkipList
{
public:
    const static unsigned int DEFAULT_MAX_RANK = 12000;
    const static unsigned int DEFAULT_MIN_RANK = 16;
    
public:
    typedef unsigned long                                           score_type;
    typedef KeyT                                                    data_type;
    typedef std::pair<data_type, score_type>                        value_type;

    typedef taf::CTCSkipList<value_type, score_type>                skip_list_type;
    typedef typename skip_list_type::iterator                       iterator;
    typedef typename skip_list_type::const_iterator                 const_iterator;
    typedef typename skip_list_type::reverse_iterator               reverse_iterator;
    typedef typename skip_list_type::const_reverse_iterator         const_reverse_iterator;
    typedef std::unordered_map<KeyT, iterator>                      hash_map_type;
    typedef typename hash_map_type::size_type                       size_type;
    
public:
    CLogicRankSkipList() : m_bIsLess(false), m_uiMaxRank(DEFAULT_MAX_RANK), m_stRankList(), m_stKeyHash(m_uiMaxRank) {};
    
    CLogicRankSkipList(unsigned int uiMaxRank) : m_bIsLess(false), m_uiMaxRank(uiMaxRank), m_stRankList(), m_stKeyHash(m_uiMaxRank)
    {
        if(uiMaxRank < DEFAULT_MIN_RANK) uiMaxRank = DEFAULT_MIN_RANK;
    };
    
    /**
     * 返回值:第一个值表示插入后的排名,0表示没有排名第二个值为true表示该用户的不存在表中,false反之。
     */
    std::pair<unsigned int, bool> insert(const data_type& stVal, score_type uiScore)
    {
        typename hash_map_type::iterator stIT = m_stKeyHash.find(stVal);
        if(stIT == m_stKeyHash.end())
        {
            if((m_stRankList.size() >= m_uiMaxRank) && !m_stRankList.empty())
            {
                if(uiScore <= m_stRankList.rend()->second)
                {
                    return (std::pair<unsigned int, bool>(0, true));
                }
            }
           
            std::pair<unsigned int, iterator> stRet = m_stRankList.insert(std::pair<data_type, score_type>(stVal, uiScore));
            m_stKeyHash[stVal] = stRet.second;
            if(m_stRankList.size() > m_uiMaxRank)
            {
                iterator stMinScoreIT = m_stRankList.end();
				--stMinScoreIT;
                
                m_stKeyHash.erase(stMinScoreIT->first);
                m_stRankList.erase(stMinScoreIT);
            }
            
            return (std::pair<unsigned int, bool>(stRet.first, true));
        }
        
        return (std::pair<unsigned int, bool>(0, false));
    }
    
    /**
     * 返回值:第一个值表示修改前的排名,第二个值表示修改后的排名。0:表示没有排名  第三个值表示被踢出的排名key
     */
    std::tuple<unsigned int, unsigned int, KeyT> UpdateOrInsert(const data_type& stVal, score_type uiScore)
    {
        KeyT kick_key = {};
        typename hash_map_type::iterator stIT = m_stKeyHash.find(stVal);
        if(stIT == m_stKeyHash.end())
        {
            if (m_stRankList.size() >= m_uiMaxRank && !m_stRankList.empty())
            {
                if(m_bIsLess && std::less<score_type>()(m_stRankList.rbegin()->second, uiScore))
                {
                    return std::make_tuple( 0, 0, kick_key );
                }
                else if(!m_bIsLess && std::greater<score_type>()(m_stRankList.rbegin()->second, uiScore))
                {
                    return std::make_tuple( 0, 0, kick_key );
                }
            }

            std::pair<unsigned int, iterator> stRet = m_stRankList.insert(std::pair<data_type, score_type>(stVal, uiScore));
            m_stKeyHash[stVal] = stRet.second;
            if (m_stRankList.size() > m_uiMaxRank)
            {
                iterator stMinScoreIT = m_stRankList.end();
				--stMinScoreIT;

                kick_key = stMinScoreIT->first;
                m_stKeyHash.erase(stMinScoreIT->first);
                m_stRankList.erase(stMinScoreIT);
            }

            return std::make_tuple( 0, stRet.first, kick_key );
        }
        else
        {
            unsigned int uiPrevRank = m_stRankList.GetRank(stIT->second);
            if(uiScore == stIT->second->second)
            {
                return std::make_tuple( uiPrevRank, uiPrevRank, kick_key );
            }
            else
            {
                m_stRankList.erase(stIT->second);
                std::pair<unsigned int, iterator> stRet = m_stRankList.insert(std::pair<data_type, score_type>(stVal, uiScore));
                stIT->second = stRet.second;

                return std::make_tuple( uiPrevRank, stRet.first, kick_key );
            }
        }
    }
    
    /**
     * 返回值:名次，0表示没有名次
     */
    unsigned int GetRank(const KeyT& stKey) const
    {
        unsigned int uiRank = 0;
        typename hash_map_type::const_iterator stIT = m_stKeyHash.find(stKey);
        if (stIT != m_stKeyHash.end())
        {
            uiRank = m_stRankList.GetRank(stIT->second);
        }
        
        return (uiRank);
    }

    unsigned int GetRank(iterator stIT) const
    {
        return m_stRankList.GetRank(stIT);
    }

    inline iterator find(const KeyT& stKey)
    {
        typename hash_map_type::const_iterator stIT = m_stKeyHash.find(stKey);
        if(stIT != m_stKeyHash.end())
        {
            return stIT->second;
        }

        return end();
    }

    inline void SetMaxRank(unsigned int uiMaxRank)
    {
        if(empty() && uiMaxRank <= DEFAULT_MAX_RANK && uiMaxRank >= DEFAULT_MIN_RANK)
        {
            m_uiMaxRank = uiMaxRank;
        }
    }

    inline void SetSortRule(bool bIsLess)
    {
        if(empty())
        {
            m_bIsLess = bIsLess;
            m_stRankList.SetCmp(bIsLess);
        }
    }

    inline iterator FindByRank(unsigned int uiRank) { return (m_stRankList.FindByRank(uiRank)); }

    inline size_type size() const { return (m_stKeyHash.size()); };
    
    inline bool empty() const { return (m_stKeyHash.empty()); };
    
    inline iterator begin() { return (m_stRankList.begin()); };
    
    inline const_iterator begin() const { return (m_stRankList.begin()); };
    
    inline iterator end() { return (m_stRankList.end()); };
    
    inline const_iterator end() const { return (m_stRankList.end()); };

    inline uint32_t max_rank() const { return m_uiMaxRank; };
    
    iterator erase(iterator stIT)
    {
        m_stKeyHash.erase(stIT->first);
        return (m_stRankList.erase(stIT));
    }
    
    size_type erase(const KeyT &stKey)
    {
        typename hash_map_type::iterator stIT = m_stKeyHash.find(stKey);
        if(stIT != m_stKeyHash.end())
        {
            m_stRankList.erase(stIT->second);
            m_stKeyHash.erase(stKey);
        }
        
        return (m_stKeyHash.size());
    }
    
private:
    bool                            m_bIsLess;
    unsigned int                    m_uiMaxRank;
    skip_list_type                  m_stRankList;
    hash_map_type                   m_stKeyHash;
};
