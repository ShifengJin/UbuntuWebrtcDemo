#ifndef WEBRTCINTERFACE_H
#define WEBRTCINTERFACE_H

#include <QString>
#include "Common.h"

class WebRTCInterface
{
public:
    WebRTCInterface();
    ~WebRTCInterface();

    void Login(QString serverUrl);
    void Logout();

    void JoinVideoRoom(int videoRoomID, std::string userName);
    void LeaveVideoRoom();

    void JoinDataChannelRoom(int dataChannelRoomID, std::string userName);
    void LeaveDataChannelRoom();
    void SendMessage(std::string message);


    static WebRTCInterface* GetInstance();

    void SetVideoWindows(WINDOWID localWindow, QVector<WINDOWID> remoteWindowID);


private:
    static WebRTCInterface* Instance;
};

#endif // WEBRTCINTERFACE_H
