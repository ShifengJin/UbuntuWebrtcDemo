#include "JanusWebSocket.h"

#include <QDebug>
#include <QTimer>

#include "Common.h"

#include "JsonTools.h"

JanusWebSocket::JanusWebSocket(QObject *parent)
{
    pWebSocket = new QWebSocket();
    pPingTimer = new QTimer();
    pPingTimeOutTimer = new QTimer();

    connect(pPingTimer, SIGNAL(timeout()), this, SLOT(onPingTimer()));
    connect(pPingTimeOutTimer, SIGNAL(timeout()), this, SLOT(onPingTimeout()));

    connect(pWebSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onWsError(QAbstractSocket::SocketError)));
    connect(pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWsMessage(QString)));

    connect(pWebSocket, SIGNAL(connected()), this, SLOT(onWebSocketConnected()));
    connect(pWebSocket, SIGNAL(disconnected()), this, SLOT(onWebSocketDisConnected()));
}

JanusWebSocket::~JanusWebSocket()
{
    disconnect(pPingTimer, SIGNAL(timeout()), this, SLOT(onPingTimer()));
    disconnect(pPingTimeOutTimer, SIGNAL(timeout()), this, SLOT(onPingTimeout()));

    disconnect(pWebSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onWsError(QAbstractSocket::SocketError)));
    disconnect(pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWsMessage(QString)));

    disconnect(pWebSocket, SIGNAL(connected()), this, SLOT(onWebSocketConnected()));
    disconnect(pWebSocket, SIGNAL(disconnected()), this, SLOT(onWebSocketDisConnected()));

    pPingTimer->stop();
    pPingTimeOutTimer->stop();
    pWebSocket->close();
    delete pPingTimer;
    delete pPingTimeOutTimer;
    delete pWebSocket;
}

void JanusWebSocket::Open(const QString &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Sec-WebSocket-Protocol", "janus-protocol");
    if(url.startsWith("wss://")){
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(config);
    }
    pWebSocket->open(request);

    pPingTimer->start(pingInterval);
}

void JanusWebSocket::Close()
{
    if(pWebSocket->state() != QAbstractSocket::SocketState::UnconnectedState){
        pWebSocket->close();
    }
    pPingTimer->stop();
    pPingTimeOutTimer->stop();
}

void JanusWebSocket::SendMessage(QString message)
{
    qDebug() << "Send : " << message;

    pWebSocket->sendTextMessage(message);
}

void JanusWebSocket::ResetTimer()
{
    pPingTimeOutTimer->stop();
    pPingTimeOutTimer->start();
}

void JanusWebSocket::RegisterReceiveMessageCallback(WEBSOCKET_RECEIVEMESSAGE_CALLBACK callback)
{
    OnWebSocketReceiveMessageCallBack = callback;
}

void JanusWebSocket::RegisterErrorCallback(WEBSOCKET_ERROR_CALLBACK callback)
{
    OnWebSocketErrorCallBack = callback;
}

void JanusWebSocket::RegisterConnectedCallback(WEBSOCKET_CONNECTED_CALLBACK callback)
{
    OnWebSocketConnectedCallBack = callback;
}

void JanusWebSocket::RegisterDisConnectedCallback(WEBSOCKET_DISCONNECTED_CALLBACK callback)
{
    OnWebSocketDisConnectedCallBack = callback;
}

void JanusWebSocket::RegisterHeartBeatCallback(WEBSOCKET_HEARTBEATINFO_CALLBACK callback)
{
    OnWebSocketHeartBeatCallBack = callback;
}

void JanusWebSocket::RegisterTimeOutCallback(WEBSOCKET_TIMEOUT_CALLBACK callback)
{
    OnWebSocketTimeOutCallBack = callback;
}

void JanusWebSocket::onWsMessage(QString textMessage)
{

    qDebug() << textMessage;

    if(OnWebSocketReceiveMessageCallBack){
        OnWebSocketReceiveMessageCallBack(textMessage);
    }
}

void JanusWebSocket::onWsError(QAbstractSocket::SocketError error)
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__ << "  error : " << error;
    if(OnWebSocketErrorCallBack){
        OnWebSocketErrorCallBack();
    }
}

void JanusWebSocket::onPingTimer()
{
    if(pWebSocket->state() == QAbstractSocket::ConnectedState){
        if(!pPingTimeOutTimer->isActive()){
            pPingTimeOutTimer->start(pingTimeout);
        }
        // 此处发送心跳检测信息 "keepalive"
        if(OnWebSocketHeartBeatCallBack){
            OnWebSocketHeartBeatCallBack();
        }


    }else{
        pPingTimeOutTimer->stop();
    }
}

void JanusWebSocket::onPingTimeout()
{
    if(pWebSocket->state() != QAbstractSocket::SocketState::UnconnectedState){
        pWebSocket->close();
    }
    pPingTimer->stop();
    pPingTimeOutTimer->stop();
    if(OnWebSocketTimeOutCallBack){
        OnWebSocketTimeOutCallBack();
    }
}

void JanusWebSocket::onWebSocketConnected()
{
    if(OnWebSocketConnectedCallBack){
        OnWebSocketConnectedCallBack();
    }
}

void JanusWebSocket::onWebSocketDisConnected()
{
    if(OnWebSocketDisConnectedCallBack){
        OnWebSocketDisConnectedCallBack();
    }
}
