#include "ConferenceManager.h"
#include "JsonTools.h"
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "WebrtcStreamVideoAudio.h"
#include "Common.h"

ConferenceManager* ConferenceManager::instance = NULL;

ConferenceManager::ConferenceManager(QObject *parent) : QObject(parent)
{
    pJanusVideoRoomManager = new JanusVideoRoomManager();

    pJanusVideoRoomManager->RegisterConnectToPeerCallBack(std::bind(&ConferenceManager::ConnectToPeer_VideoAudio, this,
                                                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    pJanusVideoRoomManager->RegisterRemoteSdpCallBack(std::bind(&ConferenceManager::onRetmoeSDP, this,
                                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    pJanusVideoRoomManager->RegisterRemoteStreamRemoveCallBack(std::bind(&ConferenceManager::onRemoteStreamRemove, this,
                                                                         std::placeholders::_1));
}

ConferenceManager::~ConferenceManager()
{
    delete pJanusVideoRoomManager;
    pJanusVideoRoomManager = NULL;

    for(auto iter = mRemoteStreamInfos_VideoAudio.begin(); iter != mRemoteStreamInfos_VideoAudio.end(); iter ++){
        iter.value().stream_VideoAudio->StopView();
        std::cout << "Stop view" << std::endl;
        delete iter.value().stream_VideoAudio.release();
    }
    mRemoteStreamInfos_VideoAudio.clear();

    for(auto iter = mRemoteStreamInfos_DataChannels.begin(); iter != mRemoteStreamInfos_DataChannels.end(); iter ++){
        iter.value().stream_DataChannels->CloseDataChannel();
        delete iter.value().stream_DataChannels.release();
    }
    mRemoteStreamInfos_DataChannels.clear();

    mRemoteWinds.clear();

    mLocalWindow = 0;

    delete LocalStream_DataChannels.release();


    ConferenceManager::instance = NULL;
}

ConferenceManager *ConferenceManager::GetInstance()
{
    if(instance == NULL){
        instance = new ConferenceManager();
    }
    return instance;
}

void ConferenceManager::Login(std::string &server)
{
    pJanusVideoRoomManager->ConnectServer(server);
}

void ConferenceManager::SetVideoWindows(unsigned long local, QVector<unsigned long> remote)
{
    mLocalWindow = local;
    mRemoteWinds.clear();
    for(auto wind : remote){
        mRemoteWinds.push_back(QPair<WINDOWID, long long>(wind, 0));
    }
}

rtc::scoped_refptr<WebrtcStreamDataChannels> ConferenceManager::GetLocalWebrtcStream_DataChannels()
{
    return LocalStream_DataChannels;
}

void ConferenceManager::SetTextRoomIDAndDisplayName(int inTextRoomID, std::string inDisplayName)
{
    mTextRoomID = inTextRoomID;
    mTextRoomDisplayName = inDisplayName;
}

void ConferenceManager::LeaveVideoRoom()
{
    for(int i = 0; i < mRemoteWinds.size(); ++ i){
        mRemoteWinds[i].second = 0;
    }
    for(auto iter = mRemoteStreamInfos_VideoAudio.begin(); iter != mRemoteStreamInfos_VideoAudio.end(); iter ++){
        iter.value().stream_VideoAudio->StopView();
        std::cout << "Stop view" << std::endl;
        delete iter.value().stream_VideoAudio.release();
    }
    mRemoteStreamInfos_VideoAudio.clear();
}

void ConferenceManager::addStreamInfo_VideoAudio(rtc::scoped_refptr<WebrtcStreamVideoAudio> remoteStream)
{
    RemoteStreamInfo_VideoAudio stream;
    stream.stream_VideoAudio = remoteStream;

    stream.canSendSDP = true;
    stream.canSendCandidate = true;

    long long peerId = remoteStream->GetHandleID();

    mRemoteStreamInfos_VideoAudio[peerId] = stream;
}

void ConferenceManager::addStreamInfo_DataChannels(rtc::scoped_refptr<WebrtcStreamDataChannels> remoteStream)
{
    RemoteStreamInfo_DataChannels stream;
    stream.stream_DataChannels = remoteStream;

    stream.canSendSDP = true;
    stream.canSendCandidate = true;

    long long peerId = remoteStream->GetHandleID();

    mRemoteStreamInfos_DataChannels[peerId] = stream;
}

void ConferenceManager::sendICEs(long long id, QVector<ConferenceManager::iceCandidate> &iceCandidateList)
{
    while(iceCandidateList.count() > 0)
    {
        auto candidate = iceCandidateList.front();
        iceCandidateList.pop_front();
        pJanusVideoRoomManager->SendCandidate(id,candidate.sdp_mid,candidate.sdp_mline_index,candidate.sdp);
    }
}


void ConferenceManager::ConnectToPeer_VideoAudio(long long peerId, bool show, bool isConnect, bool isLocal)
{
    rtc::scoped_refptr<WebrtcStreamVideoAudio> remoteStream = new rtc::RefCountedObject<WebrtcStreamVideoAudio>(peerId);
    remoteStream->SetIsLocal(isLocal);

    connect(remoteStream, SIGNAL(LocalSDP(long long, QString, QString)), this, SLOT(onLocalSDP(long long, QString, QString)));
    connect(remoteStream, SIGNAL(LocalIceCandidate(long long, QString, int, QString)), this, SLOT(onLocalIceCandidate(long long, QString, int, QString)));
    connect(remoteStream, SIGNAL(IceGatheringComplete(long long)), this, SLOT(onIceGatheringComplete(long long)));
    addStreamInfo_VideoAudio(remoteStream);

    if(show){
        for(auto &wind : mRemoteWinds){
            if(wind.second == 0){
                remoteStream->SetWindowID(wind.first);
                wind.second = peerId;
                break;
            }
        }
    }

    if(isConnect){
        remoteStream->ConnectToPeer();
    }
}

// void ConferenceManager::ConnectToPeer_DataChannels(long long peerId, bool show, bool isConnect, bool isLocal)
void ConferenceManager::ConnectToPeer_DataChannels(long long peerId)
{
    rtc::scoped_refptr<WebrtcStreamDataChannels> remoteStream = new rtc::RefCountedObject<WebrtcStreamDataChannels>(peerId);

    //if(isLocal){
        LocalStream_DataChannels = remoteStream;
    //}

    connect(remoteStream, SIGNAL(LocalSDP(long long, QString, QString)), this, SLOT(onLocalSDP(long long, QString, QString)));
    connect(remoteStream, SIGNAL(LocalIceCandidate(long long, QString, int, QString)), this, SLOT(onLocalIceCandidate(long long, QString, int, QString)));
    connect(remoteStream, SIGNAL(IceGatheringComplete(long long)), this, SLOT(onIceGatheringComplete(long long)));
    remoteStream->RegisterSendLocalInfoWhenOpenDataChannelCallBack(std::bind(&ConferenceManager::onSendLocalInfoWhenOpenDataChannel, this, std::placeholders::_1));
    addStreamInfo_DataChannels(remoteStream);
}

void ConferenceManager::onLocalSDP(long long id, QString sdp, QString type)
{
    auto itr_VA = mRemoteStreamInfos_VideoAudio.find(id);
    if(itr_VA == mRemoteStreamInfos_VideoAudio.end()){

    }else{
        auto  &remoteStreamState  = *itr_VA;
        remoteStreamState.sdp = sdp.toStdString();
        remoteStreamState.sdpType = type.toStdString();
        if(remoteStreamState.canSendSDP){
            pJanusVideoRoomManager->SendSDP(id, sdp.toStdString(), type.toStdString());
        }
    }

    auto itr_DC = mRemoteStreamInfos_DataChannels.find(id);
    if(itr_DC == mRemoteStreamInfos_DataChannels.end()){

    }else{
        auto  &remoteStreamState  = *itr_DC;
        remoteStreamState.sdp = sdp.toStdString();
        remoteStreamState.sdpType = type.toStdString();
        if(remoteStreamState.canSendSDP){
            pJanusVideoRoomManager->SendSDP(id, sdp.toStdString(), type.toStdString());
        }
    }

}

void ConferenceManager::onLocalIceCandidate(long long id, QString sdp_mid, int sdp_mlineindex, QString candidate)
{
    auto itr_VA = mRemoteStreamInfos_VideoAudio.find(id);
    if(itr_VA != mRemoteStreamInfos_VideoAudio.end()){
        auto &remoteStreamState = *itr_VA;
        auto &remoteStream = remoteStreamState.stream_VideoAudio;

        QVector<iceCandidate> &iceCandidateList = remoteStreamState.iceCandidateList;
        iceCandidateList.push_back(iceCandidate{sdp_mid,sdp_mlineindex,candidate});

        if(remoteStreamState.canSendCandidate)
        {
            sendICEs(remoteStream->GetHandleID(), iceCandidateList);
        }
    }

    auto itr_DC = mRemoteStreamInfos_DataChannels.find(id);
    if(itr_DC != mRemoteStreamInfos_DataChannels.end()){
        auto &remoteStreamState = *itr_DC;
        auto &remoteStream = remoteStreamState.stream_DataChannels;

        QVector<iceCandidate> &iceCandidateList = remoteStreamState.iceCandidateList;
        iceCandidateList.push_back(iceCandidate{sdp_mid,sdp_mlineindex,candidate});

        if(remoteStreamState.canSendCandidate)
        {
            sendICEs(remoteStream->GetHandleID(), iceCandidateList);
        }
    }

}

void ConferenceManager::onRetmoeSDP(long long id, std::string type, std::string sdp, bool isTextRoom)
{
    if(isTextRoom){
        auto itr_DC = mRemoteStreamInfos_DataChannels.find(id);
        if(itr_DC == mRemoteStreamInfos_DataChannels.end())
        {
            // ConnectToPeer_DataChannels(id, false, false, true);
            ConnectToPeer_DataChannels(id);

            itr_DC = mRemoteStreamInfos_DataChannels.find(id);
            if(itr_DC == mRemoteStreamInfos_DataChannels.end())
            {
                return ;
            }
        }
        auto &remoteStreamState = *itr_DC;
        auto &remoteStream = remoteStreamState.stream_DataChannels;

        remoteStream->SetPeerSDP(type, sdp);
    }else{
        auto itr_VA = mRemoteStreamInfos_VideoAudio.find(id);
        if(itr_VA == mRemoteStreamInfos_VideoAudio.end())
        {
            ConnectToPeer_VideoAudio(id, true, false, false);
            itr_VA = mRemoteStreamInfos_VideoAudio.find(id);
            if(itr_VA == mRemoteStreamInfos_VideoAudio.end())
            {
                return ;
            }
        }
        auto &remoteStreamState = *itr_VA;
        auto &remoteStream = remoteStreamState.stream_VideoAudio;

        remoteStream->SetPeerSDP(type, sdp);
    }
}

void ConferenceManager::onRemoteStreamRemove(long long streamId)
{
    auto itr_VA = mRemoteStreamInfos_VideoAudio.find(streamId);
    if(itr_VA != mRemoteStreamInfos_VideoAudio.end())
    {
        pJanusVideoRoomManager->DisconnectPeer(itr_VA->stream_VideoAudio->GetHandleID());
        itr_VA->stream_VideoAudio->DeletePeerConnection();
        mRemoteStreamInfos_VideoAudio.erase(itr_VA);
    }
    for(auto &wind : mRemoteWinds)
    {
        if(wind.second == streamId)
        {
            wind.second = 0;
            break;
        }
    }


    auto itr_DC = mRemoteStreamInfos_DataChannels.find(streamId);
    if(itr_DC != mRemoteStreamInfos_DataChannels.end())
    {
        pJanusVideoRoomManager->DisconnectPeer(itr_DC->stream_DataChannels->GetHandleID());
        itr_DC->stream_DataChannels->DeletePeerConnection();
        mRemoteStreamInfos_DataChannels.erase(itr_DC);
    }
}

void ConferenceManager::onIceGatheringComplete(long long id)
{
    onLocalIceCandidate(id, "end", -1, "end");
}

void ConferenceManager::onSendLocalInfoWhenOpenDataChannel(std::string &data)
{
    Json::Value msg;
    msg["textroom"] = "join";
    std::string transaction = GetRandomString(12).toStdString();
    msg["transaction"] = transaction;
    msg["room"] = mTextRoomID;
    std::string username = GetRandomString(12).toStdString();
    msg["username"] = username;
    msg["display"] = mTextRoomDisplayName.c_str();
    data = JsonValueToString(msg);
}
