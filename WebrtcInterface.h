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

    static WebRTCInterface* GetInstance();

    void SetVideoWindows(WINDOWID localWindow, QVector<WINDOWID> remoteWindowID);

    void SendMessage(std::string message);

private:
    static WebRTCInterface* Instance;
};

#endif // WEBRTCINTERFACE_H
