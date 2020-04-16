#ifndef WEBRTCSTREAMDATACHANNELS_H
#define WEBRTCSTREAMDATACHANNELS_H

#include <QObject>
#include "WebrtcStreamBase.h"

typedef std::function<void(std::string)> RECVMESSAGE_CALLBACK;
typedef std::function<void(std::string&)> SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK;

class WebrtcStreamDataChannels : public WebrtcStreamBase, public webrtc::DataChannelObserver
{
public:
    WebrtcStreamDataChannels(long long mHandleID);
    ~WebrtcStreamDataChannels();

    bool CreateDataChannel();
    void CloseDataChannel();
    bool SendDataViaDataChannel(const std::string& data);

    void RegisterRecvMessageCallBack(RECVMESSAGE_CALLBACK callback);
    void RegisterSendLocalInfoWhenOpenDataChannelCallBack(SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK callback);

    bool SetPeerSDP(std::string type, std::string sdp);

    bool initializePeerConnection();


    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);
    // The data channel state have changed.
    void OnStateChange();
    //  A data buffer was successfully received.
    void OnMessage(const webrtc::DataBuffer& buffer);
private:
    RECVMESSAGE_CALLBACK                    OnRecvMessage = nullptr;
    SENDLOCALINFOWHENOPENDATACHANNEL_CALLBACK    OnSendLocalInfoWhenOpenDataChannel;
    rtc::scoped_refptr<webrtc::DataChannelInterface> mDataChannel;
};

#endif // WEBRTCSTREAMDATACHANNELS_H
