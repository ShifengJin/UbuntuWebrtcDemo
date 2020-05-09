#ifndef JANUSPEERCONNECTION_H
#define JANUSPEERCONNECTION_H

#include <QObject>
#include <QString>
#include "JsonTools.h"
#include "JanusWebSocket.h"

class JanusPeerConnection
{
public:
    JanusPeerConnection(void *tpVideoRoomManager, JanusWebSocket *tpWebSocket, long long int tSessionId);
    ~JanusPeerConnection();

    void SetVideoRoomDisplayName(std::string name);
    void SetTextRoomDisplayName(std::string name);

    void AttachVideoRoom(int roomId);
    void AttachTextRoom(int roomId);

    void CreateVideoRoom(int roomId);
    void CreateTextRoom(int roomId);
    void DestoryVideoRoom(int roomId);
    void DestoryTextRoom(int roomId);

    void SetSubscribe(long long id);
    void SetPrivateId(long long id);
    long long GetVideoRoomHandleID();
    long long GetTextRoomHandleID();
    long long GetSubscribeID();

    void SendSDP(std::string sdp, std::string type);
    void SendSDPText(std::string sdp, std::string type);
    void SendCandidate(QString sdpMid, int sdpMLineIndex, QString candidate);

private:
    void onAttachVideoRoomJoin(const Json::Value &recvMsg);
    void onAttachVideoRoomCreate(const Json::Value &recvMsg);

    void onAttachTextRoomJoin(const Json::Value &recvMsg);
    void onAttachTextRoomCreate(const Json::Value &recvMsg);

    void joinVideoRoom();
    void joinTextRoom();

    void createVideoRoom();
    void createTextRoom();

    void onCreateVideoRoom(const Json::Value &recvMsg);
    void onCreateTextRoom(const Json::Value &recvMsg);

private:
    void               *pVideoRoomManager;
    JanusWebSocket     *pWebSocket;
    long long int       mSessionID;
    long long int       mVideoRoomHandleID;
    long long int       mTextRoomHandleID;
    long long int       mPrivateId = 0;
    long long int       mSubscribeId = 0;

    int mVideoRoomId;
    int mTextRoomId;
    std::string mVideoRoomDisplayName = "cc";
    std::string mTextRoomDisplayName = "cc";
};

#endif // JANUSPEERCONNECTION_H
