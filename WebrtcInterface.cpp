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

}

void WebRTCInterface::JoinVideoRoom(int videoRoomID, std::string userName)
{
    ConferenceManager::GetInstance()->mJanusVideoRoomManager.JoinVideoRoom(videoRoomID, userName);
}

void WebRTCInterface::LeaveVideoRoom()
{

}

void WebRTCInterface::CreateVideoRoom(int roomId)
{
    ConferenceManager::GetInstance()->mJanusVideoRoomManager.CreateVideoRoom(roomId);
}

void WebRTCInterface::DestoryVideoRoom(int roomId)
{
    ConferenceManager::GetInstance()->mJanusVideoRoomManager.DestoryVideoRoom(roomId);
}

void WebRTCInterface::JoinDataChannelRoom(int dataChannelRoomID, std::string userName)
{
    ConferenceManager::GetInstance()->SetTextRoomIDAndDisplayName(dataChannelRoomID, userName);
    ConferenceManager::GetInstance()->mJanusVideoRoomManager.JoinTextRoom(dataChannelRoomID);
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
    ConferenceManager::GetInstance()->mJanusVideoRoomManager.CreateTextRoom(roomId);
}

void WebRTCInterface::DestoryTextRoom(int roomId)
{
    ConferenceManager::GetInstance()->mJanusVideoRoomManager.DestoryTextRoom(roomId);
}
