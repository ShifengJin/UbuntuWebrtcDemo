#include "CapturerTrackSource.h"
#include "ConnecttionFactory.h"
#include <QDebug>
const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";

AlvaCapturerTrackSource* AlvaCapturerTrackSource::install = NULL;

AlvaCapturerTrackSource::AlvaCapturerTrackSource()
{
    mCapturerTrackSource = new rtc::RefCountedObject<CapturerTrackSource>();
}

void AlvaCapturerTrackSource::CreateVideoTrackAndAudioTrack()
{
    mAudioTrack = ConnecttionFactory::Get()->CreateAudioTrack(kAudioLabel, ConnecttionFactory::Get()->CreateAudioSource(cricket::AudioOptions()));

    if (mCapturerTrackSource){
        mVideoTrack = ConnecttionFactory::Get()->CreateVideoTrack(kVideoLabel, mCapturerTrackSource);
    }
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> AlvaCapturerTrackSource::GetVideoTrack()
{
    return mVideoTrack;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> AlvaCapturerTrackSource::GetAudioTrack()
{
    return mAudioTrack;
}

AlvaCapturerTrackSource *AlvaCapturerTrackSource::GetInstall()
{
    if(install == NULL){
        install = new AlvaCapturerTrackSource();
        return install;
    }
    return install;
}

CapturerTrackSource::CapturerTrackSource() : VideoTrackSource(false)
{
    const size_t kWidth = 640;
    const size_t kHeight = 480;
    const size_t kFps = 30;
    const size_t kDeviceIndex = 0;
    capturer_ = new VcmCapture(kWidth, kHeight, kFps, kDeviceIndex);
}
