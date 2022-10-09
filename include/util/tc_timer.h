#pragma once

#include <map>
#include "tc_functor.h"
#include "tc_timeprovider.h"

namespace taf
{

class CTCTimer
{
public:
    typedef struct task_info_s
    {
        task_info_s()
        {
            m_iExecInterval = 0;
            m_pFuncWrapper = NULL;
        }
        
        int                                 m_iExecInterval;
        taf::TC_FunctorWrapperInterface*    m_pFuncWrapper;
    }task_info_t;
    
    typedef std::multimap<int, task_info_t> task_container_type;
    
    typedef task_container_type::iterator iterator;
    
    typedef task_container_type::const_iterator const_iterator;
    
public:
    CTCTimer() : m_stTaskTree() {  };
    
    ~CTCTimer()
    {
        for(const_iterator stIT = m_stTaskTree.begin(); stIT != m_stTaskTree.end(); stIT ++)
        {
            delete stIT->second.m_pFuncWrapper;
        }
    }
    
    template<class ParentFunctor> void SetTimerTask(int iExecInterval, const taf::TC_FunctorWrapper<ParentFunctor>& stFunc)
    {
        int iNow = (int)taf::TC_TimeProvider::getInstance()->getNow();
        
        if(iExecInterval <= 0)
        {
            iExecInterval = 1;
        }
        
        task_info_t stTaskInfo;
        stTaskInfo.m_iExecInterval = iExecInterval;
        stTaskInfo.m_pFuncWrapper = new taf::TC_FunctorWrapper<ParentFunctor>(stFunc);
        m_stTaskTree.insert(task_container_type::value_type(iNow + iExecInterval, stTaskInfo));
    }
    
    void Schedule()
    {
        int iNow = (int)taf::TC_TimeProvider::getInstance()->getNow();
        for(iterator stIT = m_stTaskTree.begin(); stIT != m_stTaskTree.end(); )
        {
            if(stIT->first <= iNow)
            {
                int iExecInterval = stIT->second.m_iExecInterval;
                taf::TC_FunctorWrapperInterface* pFunc = stIT->second.m_pFuncWrapper;
                (*pFunc)();
                m_stTaskTree.erase(stIT ++);
                
                task_info_t stTaskInfo;
                stTaskInfo.m_iExecInterval = iExecInterval;
                stTaskInfo.m_pFuncWrapper = pFunc;
                m_stTaskTree.insert(task_container_type::value_type(iExecInterval + iNow, stTaskInfo));
            }
            else
            {
                break;
            }
        }
    }
    
private:
    task_container_type             m_stTaskTree;
};

}
