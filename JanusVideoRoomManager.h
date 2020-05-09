#ifndef JANUSVIDEOROOMMANAGER_H
#define JANUSVIDEOROOMMANAGER_H

#include "JanusWebSocket.h"
#include "JanusPeerConnection.h"

typedef std::function<void(const Json::Value &recvData)> janus_event_listener;
typedef std::function<void(long long, bool, bool, bool)> CONNECTTOPEER_CALLBACK;
typedef std::function<void(long long, std::string, std::string, bool isTextRoom)> REMOTESDP_CALLBACK;
typedef std::function<void(long long)> REMOTESTREAMREMOVE_CALLBACK;

class JanusVideoRoomManager
{
public:
    JanusVideoRoomManager();
    ~JanusVideoRoomManager();

    void ConnectServer(const std::string &server);

    void AddMessageSuccessCallback(QString tran, const janus_event_listener &func);
    void AddMessageAckCallback(QString tran, const janus_event_listener &func);


    void CreateVideoRoom(int roomId);
    void CreateTextRoom(int roomId);
    void DestoryVideoRoom(int roomId);
    void DestoryTextRoom(int roomId);

    void JoinVideoRoom(int roomId, std::string videoRoomDisplayName);
    void JoinTextRoom(int roomId);

    void RegisterConnectToPeerCallBack(CONNECTTOPEER_CALLBACK callback);
    void RegisterRemoteSdpCallBack(REMOTESDP_CALLBACK callback);
    void RegisterRemoteStreamRemoveCallBack(REMOTESTREAMREMOVE_CALLBACK callback);

    void DisConnectServer();
    bool DisconnectPeer(long long peer_id);
    void SendSDP(long long &id, std::string sdp, std::string type);
    void SendCandidate(long long &id, QString &sdpMid, int &sdpMLineIndex, QString &candidate);

private:
    void onSocketConnected();
    void onSocketDisConnected();

    void onWebSocketReceivedMessage(QString message);

    void onWebSocketHeartBeatMessage();

    void createSessionId();
    void onCreateSessionId(const Json::Value &recvmsg);



    void onHeartBeat(const Json::Value &recvmsg);


    void onMessageSuccess(const std::string &transaction, const Json::Value &object);

    void onMessageAck(const std::string &transaction, const Json::Value &object);

    void onVideoRoomEvent(const std::string &event_std, const Json::Value &object);
    void onTextRoomEvent(const std::string &event_std, const Json::Value &object);


    void onVideoRoomEventPublisher(const Json::Value &recvData);
    void onVideoRoomEventJoin(const Json::Value &recvData);

    void onVideoRoomEventSdp(const Json::Value &recvData, bool isTextRoom);
    void onVideoRoomEventAttached(const Json::Value &recvData);

    void onVideoRoomEventUnpublish(const Json::Value &recvData);
    void onVideoRoomEventEvent(const Json::Value &recvData);

    void onTextRoomEventEvent(const Json::Value &recvData);

    void onCreateVideoRoomSuccessed(unsigned int roomId);
    void onCreateVideoRoomFailed(unsigned int roomId);
    void onCreateTextRoomSuccessed(unsigned int roomId);
    void onCreateTextRoomFailed(unsigned int roomId);
private:
    JanusWebSocket          *pWebSocket;

    int                 mVideoRoomID = 1234;
    // int                 mTextRoomID = 1234;

    long long int       mSessionId = 0;
    long long int       mPrivatedId = 0;

    QMap<QString, janus_event_listener> mMessage_success_callback;
    QMap<QString, janus_event_listener> mMessage_ack_callback;

    QMap<QString, janus_event_listener> mMessage_event_videoRoom;
    QMap<QString, janus_event_listener> mMessage_event_textRoom;

    std::list<JanusPeerConnection*>      mVideoRoomPeerList;
    std::list<JanusPeerConnection*>      mTextRoomPeerList;

    JanusPeerConnection*                 pCreateVideoRoomPeer = NULL;
    JanusPeerConnection*                 pCreateTextRoomPeer = NULL;

    CONNECTTOPEER_CALLBACK               OnConnectToPeer;
    REMOTESDP_CALLBACK                   OnRemoteSdp;
    REMOTESTREAMREMOVE_CALLBACK          OnRemoteStreamRemove;

};

#endif // JANUSVIDEOROOMMANAGER_H
