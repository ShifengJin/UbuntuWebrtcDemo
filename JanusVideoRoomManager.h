#ifndef JANUSVIDEOROOMMANAGER_H
#define JANUSVIDEOROOMMANAGER_H

#include <QObject>
#include "JanusWebSocket.h"
#include "JanusPeerConnection.h"
#include "AlvaWebrtcCallBackDefine.h"

class JanusVideoRoomManager : public QObject
{
    Q_OBJECT
public:
    JanusVideoRoomManager();
    ~JanusVideoRoomManager();

    void ConnectServer(const std::string & server);
    void DisConnectServer();
    bool DisconnectPeer(long long peer_id);
    void SendSDP(long long &id, std::string sdp, std::string type);
    void SendCandidate(long long &id, QString &sdpMid, int &sdpMLineIndex, QString &candidate);

    void RegisterConnectToPeer_CallBack(CONNECTTOPEER_CALLBACK callback);
    void RegisterRemoteSdp_CallBack(REMOTESDP_CALLBACK callback);
    void RegisterRemoteStreamRemove_callBack(REMOTESTREAMREMOVE_CALLBACK callback);

private Q_SLOTS:
    // 當 mSocket 鏈接成功後觸發該函數
    void onSocketConnected();
    // 當 mSocket 斷開鏈接後觸發該函數
    void onSocketDisconnected();

private:
    // webSocket 鏈接成功後需要調用該函數來創建 sessionId
    // 第二步  第三部
    void createSessionId();

    void onCreateSessionId(const Json::Value &recvmsg);

    void join();
    void joinText();



    //第三步  第五步  第六步
    void onEventJoin(const Json::Value &recvData);

    void onEventAttached(const Json::Value &recvData);

    void onEventEvent(const Json::Value &recvData);
    void onEventEvent_TextRoom(const Json::Value &recvData);

    void onEventPublishers(const Json::Value &recvData);

    void onEventSdp(const Json::Value &recvdata, bool isTextRoom);

    void onEventUnpublish(const Json::Value &recvdata);
private:
    JanusWebSocket             mSocket;

    long long int              mSessionId = 0;
    QString                    mOpaqueId;
    long long int              mPrivateId = 0;
    std::list<JanusPeerConnection*> mPeersList;

private:
    CONNECTTOPEER_CALLBACK       OnConnectToPeer;
    REMOTESDP_CALLBACK           OnRemoteSdp;
    REMOTESTREAMREMOVE_CALLBACK  OnRemoteStreamRemove;

};

#endif // JANUSVIDEOROOMMANAGER_H
