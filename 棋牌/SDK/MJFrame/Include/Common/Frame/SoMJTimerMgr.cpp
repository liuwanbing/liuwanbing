#include "MJTimerMgr.h"

using namespace MyGame;

CSoMJTimerMgr::CSoMJTimerMgr()
{
    m_nTimerID = 0;
    m_cuTimerRemain = 0;
    m_SetRemain = 0;
}

CSoMJTimerMgr::~CSoMJTimerMgr()
{
    
}

int CSoMJTimerMgr::OnSoTimer(int cuBeginTime)
{
    if (m_nTimerID !=0)
    {
        if (m_cuTimerRemain <= 0)
        {
            UINT cuTimerID = m_nTimerID;
            
            //如果内部没有再次设置，则在这里要把定时器清掉，
            //如果内部又再次设置了定时器，则此时此值不会为0
            //使用完后主动kill掉
            KillTimer();    

            OnTimeOut(cuTimerID);
        }
        else
        {
            m_cuTimerRemain -- ;
        }
    }

    return 0;
}

void CSoMJTimerMgr::SetTimer(int nTimerID, int nTime)
{
    if ((nTimerID == 0) || (nTime < 0))
    {
        return;
    }

    m_nTimerID = nTimerID;
    m_cuTimerRemain = nTime;
    m_SetRemain = nTime;
}

void CSoMJTimerMgr::KillTimer()
{
    // 清掉
    m_cuTimerRemain = 0;
    m_SetRemain = 0;
    m_nTimerID = 0;
}

void CSoMJTimerMgr::OnTimeOut( int nTimerID )
{
    // 下面的方法用的
}

/**
*  @brief 获得当前正在运行的定时器的ID
*/
int CSoMJTimerMgr::GetTimerID()
{
    return m_nTimerID;
}

/**
*  @brief 获得当前正在运行的定时器的剩余时间， 此功能暂时没有实现
*/
int CSoMJTimerMgr::GetRemainTime()
{
    return m_cuTimerRemain;
}




