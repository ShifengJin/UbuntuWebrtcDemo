#include "JanusPeerConnection.h"
#include "Common.h"
#include "JsonTools.h"
#include "JanusVideoRoomManager.h"

JanusPeerConnection::JanusPeerConnection(void *tpVideoRoomManager, JanusWebSocket *tpWebSocket, long long int tSessionId, bool tIsVideoRoomCreate, bool tIsTextRoomCreate)
{
    pVideoRoomManager = tpVideoRoomManager;
    pWebSocket = tpWebSocket;
    mSessionID = tSessionId;
    isVideoRoomCreate = tIsVideoRoomCreate;
    isTextRoomCreate = tIsTextRoomCreate;

}

JanusPeerConnection::~JanusPeerConnection()
{

}

void JanusPeerConnection::SetVideoRoomDisplayName(std::string name)
{
    mVideoRoomDisplayName = name;
}

void JanusPeerConnection::SetTextRoomDisplayName(std::string name)
{
    mTextRoomDisplayName = name;
}
void JanusPeerConnection::AttachVideoRoom(int roomId)
{
    Json::Value msg;
    msg["janus"] = "attach";

    msg["plugin"] = "janus.plugin.videoroom";

    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);

    mVideoRoomId = roomId;

    if(isVideoRoomCreate){
        ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachVideoRoomCreate, this, std::placeholders::_1));
    }else{
        ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachVideoRoomJoin, this, std::placeholders::_1));
    }
}

void JanusPeerConnection::AttachTextRoom(int roomId)
{
    Json::Value msg;
    msg["janus"] = "attach";

    msg["plugin"] = "janus.plugin.textroom";

    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);

    mTextRoomId = roomId;
    if(isTextRoomCreate){
        ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachTextRoomCreate, this, std::placeholders::_1));
    }else{
        ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachTextRoomJoin, this, std::placeholders::_1));
    }
}

void JanusPeerConnection::SetSubscribe(long long id)
{
    mSubscribeId = id;
}

void JanusPeerConnection::SetPrivateId(long long id)
{
    mPrivateId = id;
}

long long JanusPeerConnection::GetVideoRoomHandleID()
{
    return mVideoRoomHandleID;
}

long long JanusPeerConnection::GetTextRoomHandleID()
{
    return mTextRoomHandleID;
}

long long JanusPeerConnection::GetSubscribeID()
{
    return mSubscribeId;
}

void JanusPeerConnection::onAttachVideoRoomJoin(const Json::Value &recvMsg)
{
    mVideoRoomHandleID = JsonValueToLongLong(recvMsg, "id");
    joinVideoRoom();
}

void JanusPeerConnection::onAttachVideoRoomCreate(const Json::Value &recvMsg)
{
    mVideoRoomHandleID = JsonValueToLongLong(recvMsg, "id");
    createVideoRoom();
}

void JanusPeerConnection::onAttachTextRoomJoin(const Json::Value &recvMsg)
{
    mTextRoomHandleID = JsonValueToLongLong(recvMsg, "id");

    joinTextRoom();
}

void JanusPeerConnection::onAttachTextRoomCreate(const Json::Value &recvMsg)
{
    mTextRoomHandleID = JsonValueToLongLong(recvMsg, "id");
    createTextRoom();
}

void JanusPeerConnection::joinVideoRoom()
{
    Json::Value msg;

    msg["janus"] = "message";
    msg["handle_id"] = (double)mVideoRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "join";
    body["room"] = mVideoRoomId;

    if(mSubscribeId == 0){
        body["ptype"] = "publisher";
        body["display"] = mVideoRoomDisplayName;
    }else{
        body["ptype"] = "subscriber";
        body["feed"] = (double)mSubscribeId;
        body["private_id"] = (double)mPrivateId;
    }
    msg["body"] = body;


    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}

void JanusPeerConnection::joinTextRoom()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mTextRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "setup";
    msg["body"] = body;
    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}

void JanusPeerConnection::createVideoRoom()
{
    Json::Value msg;

    msg["janus"] = "message";
    msg["handle_id"] = (double)mVideoRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "create";
    body["room"] = mVideoRoomId;

    if(mSubscribeId == 0){
        body["ptype"] = "publisher";
        body["display"] = mVideoRoomDisplayName;
    }else{
        body["ptype"] = "subscriber";
        body["feed"] = (double)mSubscribeId;
        body["private_id"] = (double)mPrivateId;
    }
    msg["body"] = body;


    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}

void JanusPeerConnection::createTextRoom()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mTextRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "create";
    msg["body"] = body;
    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}

void JanusPeerConnection::SendSDP(std::string sdp, std::string type)
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mVideoRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;
    Json::Value body;
    if(mSubscribeId == 0){
        body["request"] = "configure";
        body["audio"] = true;
        body["video"] = true;
    }else{
        body["request"] = "start";
        body["room"] = 1234;
    }

    Json::Value jsep;
    jsep["type"] = type;
    jsep["sdp"] = sdp;

    msg["body"] = body;
    msg["jsep"] = jsep;


    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);

}

void JanusPeerConnection::SendSDPText(std::string sdp, std::string type)
{
    Json::Value msg;
    msg["janus"] = "message";
    Json::Value body;
    body["request"] = "ack";

    msg["handle_id"] = (double)mTextRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value jsep;
    jsep["type"] = type;
    jsep["sdp"] = sdp;

    msg["body"] = body;
    msg["jsep"] = jsep;


    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}

void JanusPeerConnection::requestSetup()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mVideoRoomHandleID;

    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "setup";
    msg["body"] = body;


    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}

void JanusPeerConnection::SendCandidate(QString sdpMid, int sdpMLineIndex, QString candidate)
{
    Json::Value msg;
    msg["janus"] = "trickle";

    msg["handle_id"] = (double)mVideoRoomHandleID;

    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value candidateObj;
    if(sdpMid == "end")
    {
        candidateObj["completed"] = true;
    }
    else
    {
        candidateObj["candidate"] = candidate.toStdString();
        candidateObj["sdpMid"] = sdpMid.toStdString();
        candidateObj["sdpMLineIndex"] = sdpMLineIndex;
    }
    msg["candidate"] = candidateObj;


    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}
