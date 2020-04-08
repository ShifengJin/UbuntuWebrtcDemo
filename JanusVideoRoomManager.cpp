
#include "JanusVideoRoomManager.h"
#include "JanusPeerConnection.h"

#include <QDebug>
#include "AlvaJsonTools.h"
#include "CommonDefine.h"
#include "CommonFunction.h"

JanusVideoRoomManager::JanusVideoRoomManager()
{
    // 當mSocket 鏈接後與onSocketConnected函數建立鏈接
    mSocket.RegisterWebSocketConnected_CallBack(std::bind(&JanusVideoRoomManager::onSocketConnected, this));
    mSocket.RegisterWebSocketDisConnected_CallBack(std::bind(&JanusVideoRoomManager::onSocketDisconnected, this));

    // 當收到的信息爲 "janus" : "event"  ** 1. "videoroom": "joined"  2. "videoroom" : "event"  3. "videoroom" : "attached" ** 調用以下函數
    // 第三步 "janus" : "message"  ----> "janus" : "ack"  ----> "janus" : "event" "videoroom" : "joined"
    // 調用 onEventJoin 函數會執行第四步 第五步
    // "janus" : "attach" ----> "janus" : "success"
    mSocket.SetEventCallBack("joined", std::bind(&JanusVideoRoomManager::onEventJoin, this, std::placeholders::_1));
    // 第六步 "janus" : "message"  ----> "janus" : "ack"  ----> "janus" : "event" "videoroom" : "event"
    mSocket.SetEventCallBack("event", std::bind(&JanusVideoRoomManager::onEventEvent, this, std::placeholders::_1));

    mSocket.SetEventCallBack("eventTextRoom", std::bind(&JanusVideoRoomManager::onEventEvent_TextRoom, this, std::placeholders::_1));


    // 第五步 "janus" : "message"  ----> "janus" : "ack"  ----> "janus" : "event" "videoroom" : "attached"
    mSocket.SetEventCallBack("attached", std::bind(&JanusVideoRoomManager::onEventAttached, this, std::placeholders::_1));

    mOpaqueId = "videoroomtest-" + GetRandomString(12);
}

JanusVideoRoomManager::~JanusVideoRoomManager()
{

}

void JanusVideoRoomManager::ConnectServer(const std::string &server)
{
    mSocket.Open(STDSTR_TO_QTSTR(server));
}

void JanusVideoRoomManager::DisConnectServer()
{
    mSocket.Close();
}

bool JanusVideoRoomManager::DisconnectPeer(long long peer_id)
{
    return true;
}

void JanusVideoRoomManager::SendSDP(long long &id, std::string sdp, std::string type)
{
    for(auto peer : mPeersList){
        if(peer->GetHandleId() == id){
            peer->SendSDP(sdp, type);
            break;
        }
    }
}
void JanusVideoRoomManager::SendCandidate(long long &id, QString &sdpMid, int &sdpMLineIndex, QString &candidate)
{
    for(auto peer : mPeersList)
    {
        if(peer->GetHandleId() == id)
        {
            peer->SendCandidate(sdpMid, sdpMLineIndex, candidate);
            break;
        }
    }
}

void JanusVideoRoomManager::RegisterConnectToPeer_CallBack(CONNECTTOPEER_CALLBACK callback)
{
    OnConnectToPeer = callback;
}

void JanusVideoRoomManager::RegisterRemoteSdp_CallBack(REMOTESDP_CALLBACK callback)
{
    OnRemoteSdp = callback;
}

void JanusVideoRoomManager::RegisterRemoteStreamRemove_callBack(REMOTESTREAMREMOVE_CALLBACK callback)
{
    OnRemoteStreamRemove = callback;
}

void JanusVideoRoomManager::onSocketConnected()
{
    qDebug() << __FILE__ << "    " << __FUNCTION__ << "    " << __LINE__ << "  webSocket connected";
    createSessionId();
}

void JanusVideoRoomManager::onSocketDisconnected()
{
    qDebug() << __FILE__ << "    " << __FUNCTION__ << "    " << __LINE__ << "  webSocket disconnected";
}

void JanusVideoRoomManager::createSessionId()
{
    Json::Value msg;
    msg["janus"] = "create";

    // 第一步  "janus" : "create"   ----> "janus" : "success"
    mSocket.EmitMessage(msg, std::bind(&JanusVideoRoomManager::onCreateSessionId, this, std::placeholders::_1));
}

void JanusVideoRoomManager::onCreateSessionId(const Json::Value &recvmsg)
{
    mSessionId = JsonValueToLongLong(recvmsg, "id");

    mSocket.SetSessionID(mSessionId);
    join();
    joinText();
}

void JanusVideoRoomManager::join()
{
    JanusPeerConnection *peer = new JanusPeerConnection(mSocket, mOpaqueId);
    mPeersList.push_back(peer);
    // 第二步  "janus" : "attach"  ----> "janus" : "success"
    // 第三步  "janus" : "message"  ----> "janus" : "ack" ----> "janus" : "event"
    peer->AttachVideoRoom();
}

void JanusVideoRoomManager::joinText()
{
    JanusPeerConnection *peer = new JanusPeerConnection(mSocket, mOpaqueId);
    mPeersList.push_back(peer);
    // 第二步  "janus" : "attach"  ----> "janus" : "success"
    // 第三步  "janus" : "message"  ----> "janus" : "ack" ----> "janus" : "event"
    //peer->AttachVideoRoom();
    peer->AttachTextRoom();
}

void JanusVideoRoomManager::onEventSdp(const Json::Value &recvdata, bool isTextRoom)
{
    long long sender = 0;
    sender = JsonValueToLongLong(recvdata, "sender");

    Json::Value jsepObj;
    GetValueFromJsonObject(recvdata, "jsep", &jsepObj);

    std::string type;
    std::string sdp;
    GetStringFromJsonObject(jsepObj, "type", &type);
    GetStringFromJsonObject(jsepObj, "sdp", &sdp);

    if(!sdp.empty())
    {
        if(OnRemoteSdp){
            OnRemoteSdp(sender, type, sdp, isTextRoom);
        }
    }
}

void JanusVideoRoomManager::onEventUnpublish(const Json::Value &recvdata)
{
    Json::Value plugindataOjb;
    GetValueFromJsonObject(recvdata, "plugindata", &plugindataOjb);

    Json::Value dataObj;
    GetValueFromJsonObject(plugindataOjb, "data", &dataObj);

    long long unpublishId = 0;
    unpublishId = JsonValueToLongLong(dataObj, "unpublished");

    if(unpublishId != 0)
    {
        for(auto itr = mPeersList.begin();itr != mPeersList.end();itr++)
        {
            if((*itr)->GetSubscribe() == unpublishId)
            {
                if(OnRemoteStreamRemove){
                    OnRemoteStreamRemove((*itr)->GetHandleId());
                }
                mPeersList.erase(itr);
                break;
            }
        }
    }
}

void JanusVideoRoomManager::onEventJoin(const Json::Value &recvData)
{

    onEventPublishers(recvData);
    // 此处是发送本地的音视频
    long long sender = 0;
    sender = JsonValueToLongLong(recvData, "sender");
    for(auto peer : mPeersList){
        if(peer->GetHandleId() == sender){
            if(OnConnectToPeer){
                //OnConnectToPeer(sender, false, true, true);
                OnConnectToPeer(sender, false, true, false);
            }
            break;
        }
    }
}

void JanusVideoRoomManager::onEventAttached(const Json::Value &recvData)
{
    onEventSdp(recvData, false);
}

void JanusVideoRoomManager::onEventEvent(const Json::Value &recvData)
{
    onEventSdp(recvData, false);
    onEventPublishers(recvData);
    onEventUnpublish(recvData);
}

void JanusVideoRoomManager::onEventEvent_TextRoom(const Json::Value &recvData)
{
    onEventSdp(recvData, true);
}

void JanusVideoRoomManager::onEventPublishers(const Json::Value &recvData)
{
    mPrivateId = JsonValueToLongLong(recvData, "private_id");
    Json::Value plugindataOjb;
    GetValueFromJsonObject(recvData, "plugindata", &plugindataOjb);
    Json::Value dataObj;
    GetValueFromJsonObject(plugindataOjb, "data", &dataObj);

    std::vector<Json::Value> publishersArr;
    Json::Value publishers;
    GetValueFromJsonObject(dataObj, "publishers", &publishers);
    JsonArrayToValueVector(publishers, &publishersArr);

    for(const auto publishersRef : publishersArr){
        long long id = 0;
        std::string display;

        id = JsonValueToLongLong(publishersRef, "id");

        GetStringFromJsonObject(publishersRef, "display", &display);
        if(id > 0){
            JanusPeerConnection *peer = new JanusPeerConnection(mSocket, mOpaqueId);
            mPeersList.push_back(peer);
            peer->SetSubscribe(id);
            peer->SetPrivateId(mPrivateId);
            peer->AttachVideoRoom();
        }
    }
}
