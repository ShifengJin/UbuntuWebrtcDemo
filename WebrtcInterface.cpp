#include "WebrtcInterface.h"
#include "Common.h"
#include "ConferenceManager.h"

WebRTCInterface* WebRTCInterface::Instance = NULL;

WebRTCInterface::WebRTCInterface()
{

}

void WebRTCInterface::Login(QString serverUrl)
{
    std::string url = QTSTR_TO_STDSTR(serverUrl);
    ConferenceManager::GetInstance()->Login(url);
}

void WebRTCInterface::Logout()
{
    delete ConferenceManager::GetInstance();
}

void WebRTCInterface::JoinVideoRoom(int videoRoomID, std::string userName)
{
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->JoinVideoRoom(videoRoomID, userName);
}

void WebRTCInterface::LeaveVideoRoom(unsigned int videoRoomId)
{
    ConferenceManager::GetInstance()->LeaveVideoRoom();
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->LeaveVideoRoom(videoRoomId);
}

void WebRTCInterface::CreateVideoRoom(int roomId)
{
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->CreateVideoRoom(roomId);
}

void WebRTCInterface::DestoryVideoRoom(int roomId)
{
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->DestoryVideoRoom(roomId);
}

void WebRTCInterface::JoinDataChannelRoom(int dataChannelRoomID, std::string userName)
{
    ConferenceManager::GetInstance()->SetTextRoomIDAndDisplayName(dataChannelRoomID, userName);
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->JoinTextRoom(dataChannelRoomID);
}

void WebRTCInterface::LeaveDataChannelRoom()
{

}

WebRTCInterface *WebRTCInterface::GetInstance()
{
    if(Instance == NULL){
        Instance = new WebRTCInterface();
    }
    return Instance;
}

void WebRTCInterface::SetVideoWindows(unsigned long localWindow, QVector<unsigned long> remoteWindowID)
{
    ConferenceManager::GetInstance()->SetVideoWindows(localWindow, remoteWindowID);
}

void WebRTCInterface::SendMessage(std::string message)
{
    Json::Value msg;
    msg["textroom"] = "message";
    std::string transaction = GetRandomString(12).toStdString();
    msg["transaction"] = transaction;
    msg["room"] = ConferenceManager::GetInstance()->GetTextRoomID();
    msg["text"] = message;

    std::string dataChannelSendMessage = JsonValueToString(msg);

    ConferenceManager::GetInstance()->GetLocalWebrtcStream_DataChannels()->SendDataViaDataChannel(dataChannelSendMessage);
}

void WebRTCInterface::CreateTextRoom(int roomId)
{
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->CreateTextRoom(roomId);
}

void WebRTCInterface::DestoryTextRoom(int roomId)
{
    ConferenceManager::GetInstance()->pJanusVideoRoomManager->DestoryTextRoom(roomId);
}
