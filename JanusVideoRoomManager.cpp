#include <iostream>
#include "JanusVideoRoomManager.h"
#include "JanusPeerConnection.h"

#include <QDebug>
#include "JsonTools.h"
#include "Common.h"
#include "JsonTools.h"

JanusVideoRoomManager::JanusVideoRoomManager()
{
    pWebSocket = new JanusWebSocket();
    pWebSocket->RegisterConnectedCallback(std::bind(&JanusVideoRoomManager::onSocketConnected, this));
    pWebSocket->RegisterDisConnectedCallback(std::bind(&JanusVideoRoomManager::onSocketDisConnected, this));

    pWebSocket->RegisterReceiveMessageCallback(std::bind(&JanusVideoRoomManager::onWebSocketReceivedMessage, this, std::placeholders::_1));

    pWebSocket->RegisterHeartBeatCallback(std::bind(&JanusVideoRoomManager::onWebSocketHeartBeatMessage, this));


    mMessage_event_videoRoom["joined"] = std::bind(&JanusVideoRoomManager::onVideoRoomEventJoin, this, std::placeholders::_1);
    mMessage_event_videoRoom["attached"] = std::bind(&JanusVideoRoomManager::onVideoRoomEventAttached, this, std::placeholders::_1);
    mMessage_event_videoRoom["event"] = std::bind(&JanusVideoRoomManager::onVideoRoomEventEvent, this, std::placeholders::_1);

    mMessage_event_textRoom["eventevent"] = std::bind(&JanusVideoRoomManager::onTextRoomEventEvent, this, std::placeholders::_1);

}

JanusVideoRoomManager::~JanusVideoRoomManager()
{

}

void JanusVideoRoomManager::ConnectServer(const std::string &server)
{
    pWebSocket->Open(STDSTR_TO_QTSTR(server));
}

void JanusVideoRoomManager::DisConnectServer()
{
    pWebSocket->Close();
}

void JanusVideoRoomManager::AddMessageSuccessCallback(QString tran, const janus_event_listener &func)
{
    mMessage_success_callback[tran] = func;
}

void JanusVideoRoomManager::AddMessageAckCallback(QString tran, const janus_event_listener &func)
{
    mMessage_ack_callback[tran] = func;
}

void JanusVideoRoomManager::CreateVideoRoom(int roomId)
{
    if(pCreateVideoRoomPeer == NULL){
        pCreateVideoRoomPeer = new JanusPeerConnection(this, pWebSocket, mSessionId);
        pCreateVideoRoomPeer->RegisterCreateTextRoomFailed(std::bind(&JanusVideoRoomManager::onCreateTextRoomFailed, this, std::placeholders::_1));
        pCreateVideoRoomPeer->RegisterCreateVideoRoomFailed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomFailed, this, std::placeholders::_1));
        pCreateVideoRoomPeer->RegisterCreateTextRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateTextRoomSuccessed, this, std::placeholders::_1));
        pCreateVideoRoomPeer->RegisterCreateVideoRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomSuccessed, this, std::placeholders::_1));
    }
    pCreateVideoRoomPeer->CreateVideoRoom(roomId);
}

void JanusVideoRoomManager::CreateTextRoom(int roomId)
{
    if(pCreateTextRoomPeer == NULL){
        pCreateTextRoomPeer = new JanusPeerConnection(this, pWebSocket, mSessionId);
        pCreateTextRoomPeer->RegisterCreateTextRoomFailed(std::bind(&JanusVideoRoomManager::onCreateTextRoomFailed, this, std::placeholders::_1));
        pCreateTextRoomPeer->RegisterCreateVideoRoomFailed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomFailed, this, std::placeholders::_1));
        pCreateTextRoomPeer->RegisterCreateTextRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateTextRoomSuccessed, this, std::placeholders::_1));
        pCreateTextRoomPeer->RegisterCreateVideoRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomSuccessed, this, std::placeholders::_1));
    }
    pCreateTextRoomPeer->CreateTextRoom(roomId);
}

void JanusVideoRoomManager::DestoryVideoRoom(int roomId)
{
    if(pCreateVideoRoomPeer == NULL){
        pCreateVideoRoomPeer = new JanusPeerConnection(this, pWebSocket, mSessionId);
        pCreateVideoRoomPeer->RegisterCreateTextRoomFailed(std::bind(&JanusVideoRoomManager::onCreateTextRoomFailed, this, std::placeholders::_1));
        pCreateVideoRoomPeer->RegisterCreateVideoRoomFailed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomFailed, this, std::placeholders::_1));
        pCreateVideoRoomPeer->RegisterCreateTextRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateTextRoomSuccessed, this, std::placeholders::_1));
        pCreateVideoRoomPeer->RegisterCreateVideoRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomSuccessed, this, std::placeholders::_1));
    }
    pCreateVideoRoomPeer->DestoryVideoRoom(roomId);
}

void JanusVideoRoomManager::DestoryTextRoom(int roomId)
{
    if(pCreateTextRoomPeer == NULL){
        pCreateTextRoomPeer = new JanusPeerConnection(this, pWebSocket, mSessionId);
        pCreateTextRoomPeer->RegisterCreateTextRoomFailed(std::bind(&JanusVideoRoomManager::onCreateTextRoomFailed, this, std::placeholders::_1));
        pCreateTextRoomPeer->RegisterCreateVideoRoomFailed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomFailed, this, std::placeholders::_1));
        pCreateTextRoomPeer->RegisterCreateTextRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateTextRoomSuccessed, this, std::placeholders::_1));
        pCreateTextRoomPeer->RegisterCreateVideoRoomSuccessed(std::bind(&JanusVideoRoomManager::onCreateVideoRoomSuccessed, this, std::placeholders::_1));
    }
    pCreateTextRoomPeer->DestoryTextRoom(roomId);
}

void JanusVideoRoomManager::onSocketConnected()
{
    qDebug() << __FUNCTION__;
    createSessionId();
}

void JanusVideoRoomManager::onSocketDisConnected()
{
    qDebug() << __FUNCTION__;
}

void JanusVideoRoomManager::onWebSocketReceivedMessage(QString message)
{
    Json::Reader reader;
    Json::Value object;
    std::string msg = message.toStdString();
    if(reader.parse(msg, object)){

        std::string janus;
        std::string transaction;

        GetStringFromJsonObject(object, "janus", &janus);
        GetStringFromJsonObject(object, "transaction", &transaction);

        if(janus == "success"){
            onMessageSuccess(transaction, object);
        }else if(janus == "ack"){
            onMessageAck(transaction, object);
        }else if(janus == "event"){
            Json::Value plugindataObj;
            GetValueFromJsonObject(object, "plugindata", &plugindataObj);

            Json::Value dataObj;
            GetValueFromJsonObject(plugindataObj, "data", &dataObj);

            std::string janusEvent;
            bool ret = GetStringFromJsonObject(dataObj, "videoroom", &janusEvent);
            if(ret){
                onVideoRoomEvent(janusEvent, object);
            }else{
                ret = GetStringFromJsonObject(dataObj, "textroom", &janusEvent);
                if(ret){
                    onTextRoomEvent("eventevent", object);
                }
            }
        }
    }
}

void JanusVideoRoomManager::onWebSocketHeartBeatMessage()
{
    Json::Value keepliveObj;
    keepliveObj["janus"] = "keepalive";
    QString transaction = GetRandomString(12);
    keepliveObj["transaction"] = transaction.toStdString();
    keepliveObj["session_id"] = (double)mSessionId;

    QString SendMsg = JsonValueToQString(keepliveObj);
    pWebSocket->SendMessage(SendMsg);
    AddMessageAckCallback(transaction, std::bind(&JanusVideoRoomManager::onHeartBeat, this, std::placeholders::_1));
}

void JanusVideoRoomManager::createSessionId()
{
    Json::Value msg;
    msg["janus"] = "create";
    QString transaction = GetRandomString(12);
    msg["transaction"] = transaction.toStdString();

    QString SendMsg = JsonValueToQString(msg);

    pWebSocket->SendMessage(SendMsg);

    AddMessageSuccessCallback(transaction, std::bind(&JanusVideoRoomManager::onCreateSessionId, this, std::placeholders::_1));
}

void JanusVideoRoomManager::onCreateSessionId(const Json::Value &recvmsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvmsg, "data", &dataObj);
    mSessionId = JsonValueToLongLong(dataObj, "id");

    //CreateVideoRoom(mVideoRoomID);
    //CreateTextRoom(mTextRoomID);

    // JoinVideoRoom(mVideoRoomID);
    // JoinTextRoom(mTextRoomID);
}

void JanusVideoRoomManager::onHeartBeat(const Json::Value &recvmsg)
{
    Json::Value dataObj;
    GetValueFromJsonObject(recvmsg, "data", &dataObj);
    pWebSocket->ResetTimer();
}

void JanusVideoRoomManager::onMessageSuccess(const std::string &transaction, const Json::Value &object)
{
    QString tran = QString::fromStdString(transaction);
    auto itr = mMessage_success_callback.find(tran);
    if(itr != mMessage_success_callback.end()){
        auto callback = itr.value();
        mMessage_success_callback.erase(itr);
        callback(object);
    }
}

void JanusVideoRoomManager::onMessageAck(const std::string &transaction, const Json::Value &object)
{
    QString tran = QString::fromStdString(transaction);
    auto itr = mMessage_ack_callback.find(tran);
    if(itr != mMessage_ack_callback.end()){
        auto callback = itr.value();
        mMessage_ack_callback.erase(itr);
        callback(object);
    }
}

void JanusVideoRoomManager::onVideoRoomEvent(const std::string &event_std, const Json::Value &object)
{
    QString event = QString::fromStdString(event_std);
    for(auto itr = mMessage_event_videoRoom.begin(); itr != mMessage_event_videoRoom.end(); itr ++){
        if(itr.key() == event){
            itr.value()(object);
        }
    }
}

void JanusVideoRoomManager::onTextRoomEvent(const std::string &event_std, const Json::Value &object)
{
    QString event = QString::fromStdString(event_std);
    for(auto itr = mMessage_event_textRoom.begin(); itr != mMessage_event_textRoom.end(); itr ++){
        if(itr.key() == event){
            itr.value()(object);
        }
    }
}

void JanusVideoRoomManager::onVideoRoomEventPublisher(const Json::Value &recvData)
{
    mPrivatedId = JsonValueToLongLong(recvData, "private_id");
    Json::Value plugindataObj;

    GetValueFromJsonObject(recvData, "plugindata", &plugindataObj);

    Json::Value dataObj;
    GetValueFromJsonObject(plugindataObj, "data", &dataObj);

    std::vector<Json::Value> publishersArr;
    Json::Value publishers;
    GetValueFromJsonObject(dataObj, "publishers", &publishers);
    JsonArrayToValueVector(publishers, &publishersArr);

    for(const auto publishersRef : publishersArr){
        long long id = JsonValueToLongLong(publishersRef, "id");
        if(id > 0){
            JanusPeerConnection *peer = new JanusPeerConnection(this, pWebSocket, mSessionId);
            mVideoRoomPeerList.push_back(peer);
            peer->SetSubscribe(id);
            peer->SetPrivateId(mPrivatedId);
            peer->AttachVideoRoom(mVideoRoomID);
        }
    }
}

void JanusVideoRoomManager::onVideoRoomEventJoin(const Json::Value &recvData)
{
    onVideoRoomEventPublisher(recvData);

    long long sender = 0;
    sender = JsonValueToLongLong(recvData, "sender");

    for(auto peer : mVideoRoomPeerList){
        if(peer->GetVideoRoomHandleID() == sender){

            //
            if(OnConnectToPeer){
                OnConnectToPeer(sender, false, true, false);
            }

            break;
        }
    }
}

void JanusVideoRoomManager::onVideoRoomEventSdp(const Json::Value &recvData, bool isTextRoom)
{
    long long sender = 0;
    sender = JsonValueToLongLong(recvData, "sender");

    Json::Value jsepObj;
    GetValueFromJsonObject(recvData, "jsep", &jsepObj);

    std::string type;
    std::string sdp;
    GetStringFromJsonObject(jsepObj, "type", &type);
    GetStringFromJsonObject(jsepObj, "sdp", &sdp);
    if(!sdp.empty()){
        if(OnRemoteSdp){
            OnRemoteSdp(sender, type, sdp, isTextRoom);
        }
    }
}

void JanusVideoRoomManager::onVideoRoomEventAttached(const Json::Value &recvData)
{
    onVideoRoomEventSdp(recvData, false);
}

void JanusVideoRoomManager::onVideoRoomEventUnpublish(const Json::Value &recvData)
{
    Json::Value plugindataObj;
    GetValueFromJsonObject(recvData, "plugindata", &plugindataObj);

    Json::Value dataObj;
    GetValueFromJsonObject(plugindataObj, "data", &dataObj);

    long long unpublishId = 0;
    unpublishId = JsonValueToLongLong(dataObj, "unpublished");

    if(unpublishId != 0){
        for(auto itr = mVideoRoomPeerList.begin(); itr != mVideoRoomPeerList.end(); itr ++){
            if((*itr)->GetSubscribeID() == unpublishId){
                if(OnRemoteStreamRemove){
                    OnRemoteStreamRemove((*itr)->GetVideoRoomHandleID());
                }
                mVideoRoomPeerList.erase(itr);
                break;
            }
        }
    }
}

void JanusVideoRoomManager::onVideoRoomEventEvent(const Json::Value &recvData)
{
    onVideoRoomEventSdp(recvData, false);
    onVideoRoomEventPublisher(recvData);
    onVideoRoomEventUnpublish(recvData);
}

void JanusVideoRoomManager::onTextRoomEventEvent(const Json::Value &recvData)
{
    onVideoRoomEventSdp(recvData, true);
}

void JanusVideoRoomManager::onCreateVideoRoomSuccessed(unsigned int roomId)
{
    std::cout << " ====== create video room successed. roomID : " << roomId << std::endl;
}

void JanusVideoRoomManager::onCreateVideoRoomFailed(unsigned int roomId)
{
    std::cout << " ====== create video room failed. roomID : " << roomId << std::endl;
}

void JanusVideoRoomManager::onCreateTextRoomSuccessed(unsigned int roomId)
{
    std::cout << " ====== create text room successed. roomID : " << roomId << std::endl;
}

void JanusVideoRoomManager::onCreateTextRoomFailed(unsigned int roomId)
{
    std::cout << " ====== create text room failed. roomID : " << roomId << std::endl;
}

void JanusVideoRoomManager::JoinVideoRoom(int roomId, std::string videoRoomDisplayName)
{
    mVideoRoomID = roomId;
    JanusPeerConnection *peer = new JanusPeerConnection(this, pWebSocket, mSessionId);
    peer->SetVideoRoomDisplayName(videoRoomDisplayName);

    mVideoRoomPeerList.push_back(peer);
    peer->AttachVideoRoom(roomId);
}

void JanusVideoRoomManager::JoinTextRoom(int roomId)
{
    // mTextRoomID = roomId;
    JanusPeerConnection *peer = new JanusPeerConnection(this, pWebSocket, mSessionId);
    mTextRoomPeerList.push_back(peer);
    peer->AttachTextRoom(roomId);
}

void JanusVideoRoomManager::RegisterConnectToPeerCallBack(CONNECTTOPEER_CALLBACK callback)
{
    OnConnectToPeer = callback;
}

void JanusVideoRoomManager::RegisterRemoteSdpCallBack(REMOTESDP_CALLBACK callback)
{
    OnRemoteSdp = callback;
}

void JanusVideoRoomManager::RegisterRemoteStreamRemoveCallBack(REMOTESTREAMREMOVE_CALLBACK callback)
{
    OnRemoteStreamRemove = callback;
}


bool JanusVideoRoomManager::DisconnectPeer(long long peer_id)
{
    return true;
}

void JanusVideoRoomManager::SendSDP(long long &id, std::string sdp, std::string type)
{
    for(auto peer : mVideoRoomPeerList){
        if(peer->GetVideoRoomHandleID() == id){
            peer->SendSDP(sdp, type);
            break;
        }
    }

    for(auto peer : mTextRoomPeerList){
        if(peer->GetTextRoomHandleID() == id){
            peer->SendSDPText(sdp, type);
            break;
        }
    }
}
void JanusVideoRoomManager::SendCandidate(long long &id, QString &sdpMid, int &sdpMLineIndex, QString &candidate)
{
    for(auto peer : mVideoRoomPeerList)
    {
        if(peer->GetVideoRoomHandleID() == id)
        {
            peer->SendCandidate(sdpMid, sdpMLineIndex, candidate);
            break;
        }
    }
}
