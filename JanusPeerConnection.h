#ifndef JANUSPEERCONNECTION_H
#define JANUSPEERCONNECTION_H

#include <QObject>
#include <QString>
#include "JanusWebSocket.h"

class JanusPeerConnection : public QObject
{
    Q_OBJECT
public:
    JanusPeerConnection(JanusWebSocket &socket, QString opaqueId, QObject *parent = nullptr);
    ~JanusPeerConnection();

    void AttachVideoRoom();
    void AttachTextRoom();

    inline long long int GetHandleId(){return mHandleId;}

    inline void SetSubscribe(long long int id){mSubscribeId = id;}
    inline void SetPrivateId(long long int id){mPrivateId = id;}
    inline long long int GetSubscribe(){return mSubscribeId;}

    void SendSDP(std::string sdp, std::string type);
    void SendSDPText(std::string sdp, std::string type);
    void SendCandidate(QString sdpMid, int sdpMLineIndex, QString candidate);

private:
    void onAttachVideoRoom(const Json::Value &recvmsg);
    void onAttachTextRoom(const Json::Value &recvmsg);

    void join();    
    void onJoin(const Json::Value &recvmsg);

    void requestSetup();
    void onRequestSetup(const Json::Value &recvmsg);


    void onSendSDP(const Json::Value &recvData);
    void onSendCandidate(const Json::Value &recvmsg);

private:
    JanusWebSocket          &mSocket;        // 公用webSocket的引用
    //long long                   mHandleId = 0;
    long long int                   mHandleId = 0;
    QString                  mOpaqueId;
    //long long                   mPrivateId = 0;
    long long int                   mPrivateId = 0;
    //long long                   mSubscribeId = 0;
    long long int                   mSubscribeId = 0;
};

#endif // JANUSPEERCONNECTION_H
