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

    rtc::scoped_refptr<WebrtcStreamDataChannels> GetLocalWebrtcStream_DataChannels();

    void SetTextRoomIDAndDisplayName(int inTextRoomID, std::string inDisplayName);
    int GetTextRoomID() { return mTextRoomID; }
    std::string GetTextRoomDisplayName() { return mTextRoomDisplayName; }

    void LeaveVideoRoom();


public Q_SLOTS:
    void ConnectToPeer_VideoAudio(long long peerId, bool show, bool connect, bool isLocal);
    //void ConnectToPeer_DataChannels(long long peerId, bool show, bool connect, bool isLocal);
    void ConnectToPeer_DataChannels(long long peerId);

    void onLocalSDP(long long id, QString sdp, QString type);

    void onRetmoeSDP(long long id, std::string type, std::string sdp, bool isTextRoom);

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

    struct RemoteStreamInfo_VideoAudio{
    public:
        rtc::scoped_refptr<WebrtcStreamVideoAudio>    stream_VideoAudio;
        std::string sdp;
        std::string sdpType;
        QVector<iceCandidate> iceCandidateList;

        bool canSendSDP;
        bool canSendCandidate;
    };

    struct RemoteStreamInfo_DataChannels{
    public:
        rtc::scoped_refptr<WebrtcStreamDataChannels>    stream_DataChannels;
        std::string sdp;
        std::string sdpType;
        QVector<iceCandidate> iceCandidateList;

        bool canSendSDP;
        bool canSendCandidate;
    };

    static ConferenceManager* instance;
public:
    JanusVideoRoomManager                 *pJanusVideoRoomManager;
private:
    QHash<long long, RemoteStreamInfo_VideoAudio>    mRemoteStreamInfos_VideoAudio;
    QHash<long long, RemoteStreamInfo_DataChannels>    mRemoteStreamInfos_DataChannels;
    QVector<QPair<WINDOWID, long long>>   mRemoteWinds;
    WINDOWID                              mLocalWindow;

    rtc::scoped_refptr<WebrtcStreamDataChannels>    LocalStream_DataChannels;

private:
    void addStreamInfo_VideoAudio(rtc::scoped_refptr<WebrtcStreamVideoAudio> remoteStream);
    void addStreamInfo_DataChannels(rtc::scoped_refptr<WebrtcStreamDataChannels> remoteStream);

    void sendICEs(long long id, QVector<iceCandidate> &iceCandidateList);


    int mTextRoomID;
    std::string mTextRoomDisplayName;
};

#endif // QTCONFERENCEMANAGER_H
