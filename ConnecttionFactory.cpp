
#include "ConnecttionFactory.h"
#include <webrtc/api/audio_codecs/builtin_audio_decoder_factory.h>
#include <webrtc/api/audio_codecs/builtin_audio_encoder_factory.h>
#include <webrtc/api/video_codecs/builtin_video_decoder_factory.h>
#include <webrtc/api/video_codecs/builtin_video_encoder_factory.h>
#include <webrtc/modules/audio_device/include/audio_device.h>
#include <webrtc/modules/audio_processing/include/audio_processing.h>
#include <webrtc/api/create_peerconnection_factory.h>

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> ConnecttionFactory::_peer_connection_factory = NULL;

bool ConnecttionFactory::Init()
{
    static std::unique_ptr<rtc::Thread> g_worker_thread;
    static std::unique_ptr<rtc::Thread> g_networker_thread;
    static std::unique_ptr<rtc::Thread> g_signaling_thread;
    g_worker_thread.reset(new rtc::Thread());
    g_worker_thread->Start();
    g_signaling_thread.reset(new rtc::Thread());
    g_signaling_thread->Start();
    g_networker_thread.reset(new rtc::Thread());
    g_networker_thread->Start();
    _peer_connection_factory = webrtc::CreatePeerConnectionFactory(
                    g_networker_thread.get(), g_worker_thread.get(), g_signaling_thread.get(),nullptr,
                    //nullptr, nullptr, g_signaling_thread.get(),nullptr,
                    webrtc::CreateBuiltinAudioEncoderFactory(),
                    webrtc::CreateBuiltinAudioDecoderFactory(),
                    webrtc::CreateBuiltinVideoEncoderFactory(),
                    webrtc::CreateBuiltinVideoDecoderFactory(),
                    nullptr,nullptr);

    return _peer_connection_factory == nullptr;
}

void ConnecttionFactory::Unit()
{
    _peer_connection_factory = NULL;
}

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> ConnecttionFactory::Get()
{
    if (_peer_connection_factory == nullptr)
    {
        Init();
    }

    return _peer_connection_factory;
}
