#ifndef ALVATHREADSTATELISTS_H
#define ALVATHREADSTATELISTS_H

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <list>

typedef enum _ALVA_WEBRTC_SENDSIGNALLING_THREAD_TYPE_{

    NONE = 0,
    SEND_LOCAL_SDP,
    SEND_LOCAL_ICE_CANDIDATE,
    SEND_ICE_GATHERING_COMPLETE

}SENDSIGNALLINGTYPE;


class AlvaThreadStateLists
{
public:
    AlvaThreadStateLists();

    // 添加一个状态 同时信号量加一
    void AddThreadOneStateToLists(SENDSIGNALLINGTYPE tState);

    // 获取一个状态并从lists中移除  同时信号量减一  没有则阻塞
    SENDSIGNALLINGTYPE GetHeadStateOfLists();

private:

    std::list<SENDSIGNALLINGTYPE> _threadStateLists;

    sem_t _sem;

};

#endif // ALVATHREADSTATELISTS_H
