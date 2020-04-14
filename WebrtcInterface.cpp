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
    QtConferenceManager::GetInstance()->Login(url);
}

void WebRTCInterface::Logout()
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
    QtConferenceManager::GetInstance()->SetVideoWindows(localWindow, remoteWindowID);
}

void WebRTCInterface::SendMessage(std::string message)
{
    QtConferenceManager::GetInstance()->GetLocalWebrtcRemoteStream()->SendDataViaDataChannel(message);
}
