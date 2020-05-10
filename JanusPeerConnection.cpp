#include <iostream>
#include "JanusPeerConnection.h"
#include "Common.h"
#include "JsonTools.h"
#include "JanusVideoRoomManager.h"

JanusPeerConnection::JanusPeerConnection(void *tpVideoRoomManager, JanusWebSocket *tpWebSocket, long long int tSessionId)
{
    pVideoRoomManager = tpVideoRoomManager;
    pWebSocket = tpWebSocket;
    mSessionID = tSessionId;
}

JanusPeerConnection::~JanusPeerConnection()
{
    mSessionID = 0;
    mVideoRoomHandleID = 0;
    mTextRoomHandleID = 0;
    mPrivateId = 0;
    mSubscribeId = 0;

    mVideoRoomId = 0;
    mTextRoomId = 0;

    onCreateVideoRoomSuccessedCallback = NULL;
    onCreateVideoRoomFailedCallback = NULL;
    onCreateTextRoomSuccessedCallback = NULL;
    onCreateTextRoomFailedCallback = NULL;

    onDestroyVideoRoomSuccessedCallback = NULL;
    onDestroyVideoRoomFailedCallback = NULL;
    onDestroyTextRoomSuccessedCallback = NULL;
    onDestroyTextRoomFailedCallback = NULL;
}

void JanusPeerConnection::SetVideoRoomDisplayName(std::string name)
{
    mVideoRoomDisplayName = name;
}

void JanusPeerConnection::SetTextRoomDisplayName(std::string name)
{
    mTextRoomDisplayName = name;
}
void JanusPeerConnection::AttachVideoRoom(unsigned int roomId)
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


    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachVideoRoomJoin, this, std::placeholders::_1));

}

void JanusPeerConnection::AttachTextRoom(unsigned int roomId)
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

    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachTextRoomJoin, this, std::placeholders::_1));
}

#if 0
void JanusPeerConnection::LeaveVideoRoom()
{
    Json::Value msg;

    msg["janus"] = "message";
    msg["handle_id"] = (double)mVideoRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "leave";
    body["room"] = mVideoRoomId;

    msg["body"] = body;

    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);

    // ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onDestoryVideoRoom, this, std::placeholders::_1));
}

void JanusPeerConnection::LeaveTextRoom()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mTextRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "leave";
    body["room"] = mTextRoomId;
    msg["body"] = body;
    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
}
#endif

void JanusPeerConnection::CreateVideoRoom(int roomId)
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

    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachVideoRoomCreate, this, std::placeholders::_1));

}

void JanusPeerConnection::CreateTextRoom(int roomId)
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

    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachVideoRoomCreate, this, std::placeholders::_1));

}

void JanusPeerConnection::DestoryVideoRoom(int roomId)
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

    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachVideoRoomDestory, this, std::placeholders::_1));
}

void JanusPeerConnection::DestoryTextRoom(int roomId)
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

    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onAttachTextRoomDestory, this, std::placeholders::_1));
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
    Json::Value dataObj;
    GetValueFromJsonObject(recvMsg, "data", &dataObj);
    mVideoRoomHandleID = JsonValueToLongLong(dataObj, "id");
    joinVideoRoom();
}

void JanusPeerConnection::onAttachVideoRoomCreate(const Json::Value &recvMsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvMsg, "data", &dataObj);
    mVideoRoomHandleID = JsonValueToLongLong(dataObj, "id");
    createVideoRoom();
}

void JanusPeerConnection::onAttachTextRoomJoin(const Json::Value &recvMsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvMsg, "data", &dataObj);
    mTextRoomHandleID = JsonValueToLongLong(dataObj, "id");
    joinTextRoom();
}

void JanusPeerConnection::onAttachTextRoomCreate(const Json::Value &recvMsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvMsg, "data", &dataObj);
    mTextRoomHandleID = JsonValueToLongLong(dataObj, "id");
    createTextRoom();
}

void JanusPeerConnection::onAttachVideoRoomDestory(const Json::Value &recvMsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvMsg, "data", &dataObj);
    mVideoRoomHandleID = JsonValueToLongLong(dataObj, "id");
    destoryVideoRoom();
}

void JanusPeerConnection::onAttachTextRoomDestory(const Json::Value &recvMsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvMsg, "data", &dataObj);
    mTextRoomHandleID = JsonValueToLongLong(dataObj, "id");
    destoryTextRoom();
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

    msg["body"] = body;

    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);

    // 此处添加创建房间成功或失败的回调函数
    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onCreateVideoRoom, this, std::placeholders::_1));
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
    body["room"] = mTextRoomId;
    msg["body"] = body;
    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);

    // 此处添加创建房间成功或失败的回调函数
    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onCreateTextRoom, this, std::placeholders::_1));
}

void JanusPeerConnection::destoryVideoRoom()
{
    Json::Value msg;

    msg["janus"] = "message";
    msg["handle_id"] = (double)mVideoRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "destroy";
    body["room"] = mVideoRoomId;

    msg["body"] = body;

    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
    // 此处添加创建房间成功或失败的回调函数
    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onDestoryVideoRoom, this, std::placeholders::_1));
}

void JanusPeerConnection::destoryTextRoom()
{
    Json::Value msg;
    msg["janus"] = "message";
    msg["handle_id"] = (double)mTextRoomHandleID;
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    msg["session_id"] = (double)mSessionID;

    Json::Value body;
    body["request"] = "destroy";
    body["room"] = mTextRoomId;
    msg["body"] = body;
    QString SendMsg = JsonValueToQString(msg);
    pWebSocket->SendMessage(SendMsg);
    // 此处添加创建房间成功或失败的回调函数
    ((JanusVideoRoomManager*)pVideoRoomManager)->AddMessageSuccessCallback(transaction, std::bind(&JanusPeerConnection::onDestoryTextRoom, this, std::placeholders::_1));
}

void JanusPeerConnection::onCreateVideoRoom(const Json::Value &recvMsg)
{
    Json::Value plugindata;
    GetValueFromJsonObject(recvMsg, "plugindata", &plugindata);

    Json::Value data;
    GetValueFromJsonObject(plugindata, "data", &data);

    std::string videoroom;
    GetStringFromJsonObject(data, "videoroom", &videoroom);

    unsigned int CreatedRoomId = mVideoRoomId;
    if(videoroom == "event"){
        // 表明創建房間失敗
        if(onCreateVideoRoomFailedCallback){
            onCreateVideoRoomFailedCallback(CreatedRoomId);
        }
    }else if(videoroom == "created"){
        // 表明創建房間成功
        GetUIntFromJsonObject(data, "room", &CreatedRoomId);
        if(onCreateVideoRoomSuccessedCallback){
            onCreateVideoRoomSuccessedCallback(CreatedRoomId);
        }
    }
}

void JanusPeerConnection::onCreateTextRoom(const Json::Value &recvMsg)
{
    Json::Value plugindata;
    GetValueFromJsonObject(recvMsg, "plugindata", &plugindata);

    Json::Value data;
    GetValueFromJsonObject(plugindata, "data", &data);

    std::string textroom;
    GetStringFromJsonObject(data, "textroom", &textroom);

    unsigned int CreatedRoomId = mTextRoomId;
    if(textroom == "error"){
        // 表明創建房間失敗
        if(onCreateTextRoomFailedCallback){
            onCreateTextRoomFailedCallback(CreatedRoomId);
        }
    }else if(textroom == "created"){
        // 表明創建房間成功
        GetUIntFromJsonObject(data, "room", &CreatedRoomId);
        if(onCreateTextRoomSuccessedCallback){
            onCreateTextRoomSuccessedCallback(CreatedRoomId);
        }
    }
}

void JanusPeerConnection::onDestoryVideoRoom(const Json::Value &recvMsg)
{
    Json::Value plugindata;
    GetValueFromJsonObject(recvMsg, "plugindata", &plugindata);

    Json::Value data;
    GetValueFromJsonObject(plugindata, "data", &data);

    std::string videoroom;
    GetStringFromJsonObject(data, "videoroom", &videoroom);

    unsigned int CreatedRoomId = mVideoRoomId;
    if(videoroom == "event"){
        // 表明销毁房間失敗
        if(onDestroyVideoRoomFailedCallback){
            onDestroyVideoRoomFailedCallback(CreatedRoomId);
        }
    }else if(videoroom == "destroyed"){
        // 表明销毁房間成功
        GetUIntFromJsonObject(data, "room", &CreatedRoomId);
        if(onDestroyVideoRoomSuccessedCallback){
            onCreateVideoRoomSuccessedCallback(CreatedRoomId);
        }
    }
}

void JanusPeerConnection::onDestoryTextRoom(const Json::Value &recvMsg)
{
    Json::Value plugindata;
    GetValueFromJsonObject(recvMsg, "plugindata", &plugindata);

    Json::Value data;
    GetValueFromJsonObject(plugindata, "data", &data);

    std::string textroom;
    GetStringFromJsonObject(data, "textroom", &textroom);

    unsigned int CreatedRoomId = mTextRoomId;
    if(textroom == "error"){
        // 表明創建房間失敗
        if(onDestroyTextRoomFailedCallback){
            onDestroyTextRoomFailedCallback(CreatedRoomId);
        }
    }else if(textroom == "destroyed"){
        // 表明創建房間成功
        GetUIntFromJsonObject(data, "room", &CreatedRoomId);
        if(onDestroyTextRoomSuccessedCallback){
            onDestroyTextRoomSuccessedCallback(CreatedRoomId);
        }
    }
}

void JanusPeerConnection::RegisterCreateVideoRoomSuccessed(CREATEROOM_CALLBACK callback)
{
    onCreateVideoRoomSuccessedCallback = callback;
}

void JanusPeerConnection::RegisterCreateVideoRoomFailed(CREATEROOM_CALLBACK callback)
{
    onCreateVideoRoomFailedCallback = callback;
}

void JanusPeerConnection::RegisterCreateTextRoomSuccessed(CREATEROOM_CALLBACK callback)
{
    onCreateTextRoomSuccessedCallback = callback;
}

void JanusPeerConnection::RegisterCreateTextRoomFailed(CREATEROOM_CALLBACK callback)
{
    onCreateTextRoomFailedCallback = callback;
}

void JanusPeerConnection::RegisterDestroyVideoRoomSuccessed(CREATEROOM_CALLBACK callback)
{
    onDestroyVideoRoomSuccessedCallback = callback;
}

void JanusPeerConnection::RegisterDestroyVideoRoomFailed(CREATEROOM_CALLBACK callback)
{
    onDestroyVideoRoomFailedCallback = callback;
}

void JanusPeerConnection::RegisterDestroyTextRoomSuccessed(CREATEROOM_CALLBACK callback)
{
    onDestroyTextRoomSuccessedCallback = callback;
}

void JanusPeerConnection::RegisterDestroyTextRoomFailed(CREATEROOM_CALLBACK callback)
{
    onDestroyTextRoomFailedCallback = callback;
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
        body["room"] = mVideoRoomId;
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
