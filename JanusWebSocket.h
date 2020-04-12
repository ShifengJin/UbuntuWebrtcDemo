#ifndef JANUSWEBSOCKET_H
#define JANUSWEBSOCKET_H

#include <QObject>
#include <QtWebSockets>

typedef std::function<void(QString)> WEBSOCKET_RECEIVEMESSAGE_CALLBACK;
typedef std::function<void(void)>    WEBSOCKET_ERROR_CALLBACK;
typedef std::function<void(void)>    WEBSOCKET_CONNECTED_CALLBACK;
typedef std::function<void(void)>    WEBSOCKET_DISCONNECTED_CALLBACK;
typedef std::function<void(void)>    WEBSOCKET_HEARTBEATINFO_CALLBACK;
typedef std::function<void(void)>    WEBSOCKET_TIMEOUT_CALLBACK;


class JanusWebSocket : public QObject
{
    Q_OBJECT
public:
    explicit JanusWebSocket(QObject * parent = Q_NULLPTR);
    ~JanusWebSocket();

    void Open(const QString &url);
    void Close();
    void SendMessage(QString message);
    void ResetTimer();

    void RegisterReceiveMessageCallback(WEBSOCKET_RECEIVEMESSAGE_CALLBACK callback);

    void RegisterErrorCallback(WEBSOCKET_ERROR_CALLBACK callback);

    void RegisterConnectedCallback(WEBSOCKET_CONNECTED_CALLBACK callback);

    void RegisterDisConnectedCallback(WEBSOCKET_DISCONNECTED_CALLBACK callback);

    void RegisterHeartBeatCallback(WEBSOCKET_HEARTBEATINFO_CALLBACK callback);

    void RegisterTimeOutCallback(WEBSOCKET_TIMEOUT_CALLBACK callback);

private Q_SLOTS:

    /**
     * @brief websocket 收到信息后调用该函数
     * @param textMessage : 收到的信息
     */
    void onWsMessage(QString textMessage);

    /**
     * @brief websocket 发生错误后调用该函数
     * @param error 错误信息
     */
    void onWsError(QAbstractSocket::SocketError error);

    /**
     * @brief 心跳检测函数
     */
    void onPingTimer();

    /**
     * @brief 连接超时调用该函数
     */
    void onPingTimeout();

    /**
     * @brief websocket 连接成功后调用该函数
     */
    void onWebSocketConnected();

    /**
     * @brief websocket 断开连接后调用该函数
    */
    void onWebSocketDisConnected();

private:
    QWebSocket               *pWebSocket;

    uint                      pingInterval = 25000;
    uint                      pingTimeout = 60000;
    QTimer                   *pPingTimer;
    QTimer                   *pPingTimeOutTimer;

    WEBSOCKET_RECEIVEMESSAGE_CALLBACK      OnWebSocketReceiveMessageCallBack;
    WEBSOCKET_ERROR_CALLBACK               OnWebSocketErrorCallBack;
    WEBSOCKET_CONNECTED_CALLBACK           OnWebSocketConnectedCallBack;
    WEBSOCKET_DISCONNECTED_CALLBACK        OnWebSocketDisConnectedCallBack;
    WEBSOCKET_HEARTBEATINFO_CALLBACK       OnWebSocketHeartBeatCallBack;
    WEBSOCKET_TIMEOUT_CALLBACK             OnWebSocketTimeOutCallBack;
};

#endif // JANUSWEBSOCKET_H
