#ifndef JANUSPEERCONNECTION_H
#define JANUSPEERCONNECTION_H

#include <QObject>
#include <QString>
#include "JsonTools.h"
#include "JanusWebSocket.h"

typedef std::function<void(unsigned int)> CREATEROOM_CALLBACK;

class JanusPeerConnection
{
public:
    JanusPeerConnection(void *tpVideoRoomManager, JanusWebSocket *tpWebSocket, long long int tSessionId);
    ~JanusPeerConnection();

    void SetVideoRoomDisplayName(std::string name);
    void SetTextRoomDisplayName(std::string name);

    void AttachVideoRoom(unsigned int roomId);
    void AttachTextRoom(unsigned int roomId);
#if 0
    void LeaveVideoRoom();
    void LeaveTextRoom();
#endif
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

    unsigned int GetVideoRoomID() { return mVideoRoomId; }
    unsigned int GetTextRoomID() { return mTextRoomId; }

private:
    void onAttachVideoRoomJoin(const Json::Value &recvMsg);
    void onAttachVideoRoomCreate(const Json::Value &recvMsg);

    void onAttachTextRoomJoin(const Json::Value &recvMsg);
    void onAttachTextRoomCreate(const Json::Value &recvMsg);

    void onAttachVideoRoomDestory(const Json::Value &recvMsg);
    void onAttachTextRoomDestory(const Json::Value &recvMsg);

    void joinVideoRoom();
    void joinTextRoom();

    void createVideoRoom();
    void createTextRoom();

    void destoryVideoRoom();
    void destoryTextRoom();

    void onCreateVideoRoom(const Json::Value &recvMsg);
    void onCreateTextRoom(const Json::Value &recvMsg);

    void onDestoryVideoRoom(const Json::Value &recvMsg);
    void onDestoryTextRoom(const Json::Value &recvMsg);
private:
    void               *pVideoRoomManager;
    JanusWebSocket     *pWebSocket;
    long long int       mSessionID;
    long long int       mVideoRoomHandleID;
    long long int       mTextRoomHandleID;
    long long int       mPrivateId = 0;
    long long int       mSubscribeId = 0;

    unsigned int mVideoRoomId;
    unsigned int mTextRoomId;
    std::string mVideoRoomDisplayName = "cc";
    std::string mTextRoomDisplayName = "cc";

public:
    void RegisterCreateVideoRoomSuccessed(CREATEROOM_CALLBACK callback);
    void RegisterCreateVideoRoomFailed(CREATEROOM_CALLBACK callback);
    void RegisterCreateTextRoomSuccessed(CREATEROOM_CALLBACK callback);
    void RegisterCreateTextRoomFailed(CREATEROOM_CALLBACK callback);

    void RegisterDestroyVideoRoomSuccessed(CREATEROOM_CALLBACK callback);
    void RegisterDestroyVideoRoomFailed(CREATEROOM_CALLBACK callback);
    void RegisterDestroyTextRoomSuccessed(CREATEROOM_CALLBACK callback);
    void RegisterDestroyTextRoomFailed(CREATEROOM_CALLBACK callback);
private:
    CREATEROOM_CALLBACK          onCreateVideoRoomSuccessedCallback = NULL;
    CREATEROOM_CALLBACK          onCreateVideoRoomFailedCallback = NULL;
    CREATEROOM_CALLBACK          onCreateTextRoomSuccessedCallback = NULL;
    CREATEROOM_CALLBACK          onCreateTextRoomFailedCallback = NULL;

    CREATEROOM_CALLBACK          onDestroyVideoRoomSuccessedCallback = NULL;
    CREATEROOM_CALLBACK          onDestroyVideoRoomFailedCallback = NULL;
    CREATEROOM_CALLBACK          onDestroyTextRoomSuccessedCallback = NULL;
    CREATEROOM_CALLBACK          onDestroyTextRoomFailedCallback = NULL;

};

#endif // JANUSPEERCONNECTION_H
