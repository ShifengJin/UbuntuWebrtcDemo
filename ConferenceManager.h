#ifndef CONFERENCEMANAGER_H
#define CONFERENCEMANAGER_H

#include <QObject>

#include "JanusVideoRoomManager.h"
#include "WebrtcStreamVideoAudio.h"
#include "WebrtcStreamDataChannels.h"

class ConferenceManager : public QObject
{
    Q_OBJECT
public:
    ConferenceManager(QObject *parent = Q_NULLPTR);
    ~ConferenceManager();

    static ConferenceManager* GetInstance();

    void Login(std::string &server);

    void SetVideoWindows(WINDOWID local, QVector<WINDOWID> remote);

    rtc::scoped_refptr<WebrtcStreamVideoAudio> GetLocalWebrtcRemoteStream();
    // rtc::scoped_refptr<WebrtcStreamDataChannels> GetLocalWebrtcRemoteStream();
public Q_SLOTS:
    void ConnectToPeer(long long peerId, bool show, bool connect, bool isLocal);

    //void onLocalSDP(long long id, std::string sdp, std::string type);
    void onLocalSDP(long long id, QString sdp, QString type);

    void onRetmoeSDP(long long id, std::string type, std::string sdp, bool isTextRoom);

    void onRetmoeIce(long long id, QString sdp_mid, int sdp_mlineindex, QString candidate);

    void onLocalIceCandidate(long long id, QString sdp_mid, int sdp_mlineindex, QString candidate);

    void onRemoteStreamRemove(long long streamId);

    void onIceGatheringComplete(long long id);

    void onSendLocalInfoWhenOpenDataChannel(std::string &data);
private:

    struct iceCandidate{
        QString sdp_mid;
        int sdp_mline_index;
        QString sdp;
    };

    struct RemoteStreamInfo{
    public:
        //rtc::scoped_refptr<WebrtcRemoteStream>    stream;
        rtc::scoped_refptr<WebrtcStreamVideoAudio>    stream;
        // rtc::scoped_refptr<WebrtcStreamDataChannels>    stream;
        std::string sdp;
        std::string sdpType;
        QVector<iceCandidate> iceCandidateList;

        bool canSendSDP;
        bool canSendCandidate;
    };

    static ConferenceManager* instance;

    JanusVideoRoomManager                 mJanusVideoRoomManager;
    QHash<long long, RemoteStreamInfo>    mRemoteStreamInfos;
    QVector<QPair<WINDOWID, long long>>   mRemoteWinds;
    WINDOWID                              mLocalWindow;
    rtc::scoped_refptr<WebrtcStreamVideoAudio>    LocalStream;
    //rtc::scoped_refptr<WebrtcStreamDataChannels>    LocalStream;

private:
    // void addStreamInfo(rtc::scoped_refptr<WebrtcRemoteStream> remoteStream);
    void addStreamInfo(rtc::scoped_refptr<WebrtcStreamVideoAudio> remoteStream);
    // void addStreamInfo(rtc::scoped_refptr<WebrtcStreamDataChannels> remoteStream);

    void sendICEs(long long id, QVector<iceCandidate> &iceCandidateList);
};

#endif // QTCONFERENCEMANAGER_H
