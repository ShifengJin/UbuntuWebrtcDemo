#ifndef JANUSWEBSOCKET_H
#define JANUSWEBSOCKET_H

#include <QObject>
#include <QtWebSockets>
#include "CommonFunction.h"

#include <QObject>
#include <QtWebSockets>

typedef std::function<void(const Json::Value &recvData)> janus_event_listener;
typedef std::function<void(void)> WEBSOCKETCONNECTED_CALLBACK;
typedef std::function<void(void)> WEBSOCKETDISCONNECTED_CALLBACK;


class JanusWebSocket : public QObject
{
    Q_OBJECT
public:
    explicit JanusWebSocket(QObject * parent = Q_NULLPTR);
    ~JanusWebSocket();

    bool Open(const QString &url);
    void Close();

    void EmitMessage(Json::Value &message, const janus_event_listener &func);

    void SetEventCallBack(const QString &event_name, const janus_event_listener &func);

    void SetSessionID(const long long &id);

    void RegisterWebSocketConnected_CallBack(WEBSOCKETCONNECTED_CALLBACK callback);
    void RegisterWebSocketDisConnected_CallBack(WEBSOCKETDISCONNECTED_CALLBACK callback);

private Q_SLOTS:

    /*
     * websocket 收到信息後，此函數會觸發
    */
    void onWsMessage(QString textMessage);

    /*
     * websocket 發生錯誤會觸發此函數
    */
    void onWsError(QAbstractSocket::SocketError error);

    /*
     * 設定時間,保證websocket 一直處於鏈接狀態，同時對onPingTimeout狀態清除
    */
    void onPingTimer();

    /*
     * 設定時間,如果websocket 在該時間內沒有連同,則會觸發該函數
    */
    void onPingTimeout();

    void onWebSocketConnected();

    void onWebSocketDisConnected();

private:
    /*
     * 通過websocket 發送數據
    */
    QString doEmitMessage(Json::Value &message);

    /*
     *
    */
    void onPingAck();
    /*
     * onEvent  onNotify  onMsgAck 三個函數在onWsMessage 函數中調用,
     * 即，對於收到的不同信息進行不同的處理
     */
    void onEvent(const std::string &event, const Json::Value &object);

    void onNotify(const std::string &notify_std, const Json::Value &object);

    void onMsgAck(const std::string &transaction, const Json::Value &object);

private:
    QWebSocket       *pWebSocket;

    QMap<QString, janus_event_listener> mMessageCallbacks;
    QMap<QString, janus_event_listener> mEventCallbacks;
    QMap<QString, janus_event_listener> mNotifyCallbacks;

    uint              pingInterval = 25000;
    uint              pingTimeout  = 60000;
    QTimer           *pPingTimer;
    QTimer           *pPingTimeOutTimer;

    WEBSOCKETCONNECTED_CALLBACK         OnWebSocketConnected;
    WEBSOCKETDISCONNECTED_CALLBACK      OnWebSocketDisConnected;

    long long int mSessionID;
};

#endif // JANUSWEBSOCKET_H
