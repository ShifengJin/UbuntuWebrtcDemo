#include "JanusWebSocket.h"

#include <QDebug>
#include <QTimer>

#include "CommonFunction.h"
#include "CommonDefine.h"

#include "AlvaJsonTools.h"

JanusWebSocket::JanusWebSocket(QObject *parent)
{
    mSessionID = 0;
    pWebSocket = new QWebSocket();
    pPingTimer = new QTimer();
    pPingTimeOutTimer = new QTimer();

    connect(pPingTimer, SIGNAL(timeout()), this, SLOT(onPingTimer()));

    connect(pPingTimeOutTimer, SIGNAL(timeout()), this, SLOT(onPingTimeout()));

    connect(pWebSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onWsError(QAbstractSocket::SocketError)));
    connect(pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onWsMessage(QString)));

    connect(pWebSocket, SIGNAL(connected()), this, SLOT(onWebSocketConnected()));
    connect(pWebSocket, SIGNAL(disconnected()), this, SLOT(onWebSocketDisConnected()));

    pPingTimer->start(pingInterval);
}

JanusWebSocket::~JanusWebSocket()
{
    this->Close();
    delete pPingTimer;
    delete pPingTimeOutTimer;
    delete pWebSocket;
}

bool JanusWebSocket::Open(const QString &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Sec-WebSocket-Protocol","janus-protocol");

    if(url.startsWith("wss://"))
    {
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(config);
    }

    pWebSocket->open(request);
    qDebug() << "open : " << url;
    return true;
}

void JanusWebSocket::Close()
{
    if(pWebSocket->state() != QAbstractSocket::SocketState::UnconnectedState){
        pWebSocket->close();
    }

    pPingTimer->stop();
    pPingTimeOutTimer->stop();
}

void JanusWebSocket::EmitMessage(Json::Value &message, const janus_event_listener &func)
{
    QString transaction = doEmitMessage(message);
    mMessageCallbacks[transaction] = func;
}

void JanusWebSocket::SetEventCallBack(const QString &event_name, const janus_event_listener &func)
{
    mEventCallbacks[event_name] = func;
}

void JanusWebSocket::SetSessionID(const long long &id)
{
    this->mSessionID = id;
}

void JanusWebSocket::RegisterWebSocketConnected_CallBack(WEBSOCKETCONNECTED_CALLBACK callback)
{
    OnWebSocketConnected = callback;
}

void JanusWebSocket::RegisterWebSocketDisConnected_CallBack(WEBSOCKETDISCONNECTED_CALLBACK callback)
{
    OnWebSocketDisConnected = callback;
}

void JanusWebSocket::onWsMessage(QString textMessage)
{
    std::thread::id threadid = std::this_thread::get_id();
    std::stringstream sin;
    sin << threadid;

    //qDebug() << "================================> onWsMessage   thread id : " << QString::fromStdString(sin.str());


    Json::Reader reader;
    Json::Value object;
    std::string message = textMessage.toStdString();
    if(reader.parse(message, object))
    {
        qDebug() << "onWsMessage : " << QTSTR_TO_STDSTR(textMessage);
        std::string janus;
        std::string transaction;
        GetStringFromJsonObject(object, "janus", &janus);
        GetStringFromJsonObject(object, "transaction", &transaction);

        if(janus == "webrtcup"
           || janus == "media"
           || janus == "hangup")
        {
            onNotify(janus, object);
        }
        else if(janus == "event")
        {
            Json::Value plugindataObj;
            GetValueFromJsonObject(object, "plugindata", &plugindataObj);

            Json::Value dataObj;
            GetValueFromJsonObject(plugindataObj, "data", &dataObj);

            std::string janusEvent;

            bool ret = GetStringFromJsonObject(dataObj, "videoroom", &janusEvent);
            if(ret){
                onEvent(janusEvent,  object);
            }else{
                ret = GetStringFromJsonObject(dataObj, "textroom", &janusEvent);
                if(ret){
                    onEvent("eventTextRoom",  object);
                }
            }
        }
        else if(janus == "ack"
             || janus == "success")
        {
            // "janus" : "create" --> "janus" : "success"    第一步
            Json::Value dataObj;
            GetValueFromJsonObject(object, "data", &dataObj);
            onMsgAck(transaction, dataObj);
        }
    }
}

void JanusWebSocket::onWsError(QAbstractSocket::SocketError error)
{
    qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << "  " << __LINE__ << "Error occurred!";
}

void JanusWebSocket::onPingTimer()
{
    if(pWebSocket->state() == QAbstractSocket::ConnectedState){
        if(!pPingTimeOutTimer->isActive()){
            pPingTimeOutTimer->start(pingTimeout);
        }
        Json::Value keepliveObj;
        keepliveObj["janus"] = "keepalive";
        EmitMessage(keepliveObj, std::bind(&JanusWebSocket::onPingAck, this));
    }else{
        pPingTimeOutTimer->stop();
    }
}

void JanusWebSocket::onPingTimeout()
{
    this->Close();
}

void JanusWebSocket::onWebSocketConnected()
{
    if(OnWebSocketConnected){
        OnWebSocketConnected();
    }
}

void JanusWebSocket::onWebSocketDisConnected()
{
    if(OnWebSocketDisConnected){
        OnWebSocketDisConnected();
    }
}

QString JanusWebSocket::doEmitMessage(Json::Value &message)
{
    QString transaction = GetRandomString(12);
    message["transaction"] = transaction.toStdString();
    if(mSessionID != 0){
        message["session_id"] = (double)mSessionID;
    }

    std::string json_std_str = JsonValueToString(message);
    QString json_Qt_str = QString::fromStdString(json_std_str);
    QJsonObject sendQJsonObjec;
    bool ret = ConvertToJson(json_Qt_str, sendQJsonObjec);
    qDebug() << "Send : " << QTSTR_TO_STDSTR(QString(QJsonDocument(sendQJsonObjec).toJson(QJsonDocument::Compact)));
    pWebSocket->sendTextMessage(QString(QJsonDocument(sendQJsonObjec).toJson(QJsonDocument::Compact)));

    return transaction;
}

void JanusWebSocket::onPingAck()
{
    pPingTimeOutTimer->stop();
    pPingTimeOutTimer->start();
}

void JanusWebSocket::onEvent(const std::string &event_std, const Json::Value &object)
{
    QString event = QString::fromStdString(event_std);
    for(auto itr = mEventCallbacks.begin(); itr != mEventCallbacks.end(); itr++)
    {
        if(itr.key() == event)
        {
            itr.value()(object);
        }
    }
}

void JanusWebSocket::onNotify(const std::string &notify_std, const Json::Value &object)
{
    QString notify = QString::fromStdString(notify_std);
    auto itr = mNotifyCallbacks.find(notify);
    if(itr != mNotifyCallbacks.end())
    {
        auto callback = itr.value();
        callback(object);
        return;
    }
}

void JanusWebSocket::onMsgAck(const std::string &transaction_std, const Json::Value &object)
{
    QString transaction = QString::fromStdString(transaction_std);
    auto itr = mMessageCallbacks.find(transaction);
    if(itr != mMessageCallbacks.end()){
        auto callback = itr.value();
        mMessageCallbacks.erase(itr);
        callback(object);
    }
}
