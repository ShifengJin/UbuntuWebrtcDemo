#include "AlvaThreadStateLists.h"

AlvaThreadStateLists::AlvaThreadStateLists()
{
    sem_init(&_sem, 0, 0);
}

void AlvaThreadStateLists::AddThreadOneStateToLists(SENDSIGNALLINGTYPE tState)
{
    _threadStateLists.push_back(tState);
    sem_post(&_sem);
}

SENDSIGNALLINGTYPE AlvaThreadStateLists::GetHeadStateOfLists()
{
    sem_wait(&_sem);
    SENDSIGNALLINGTYPE outType = _threadStateLists.front();
    _threadStateLists.pop_front();
    return outType;
}
