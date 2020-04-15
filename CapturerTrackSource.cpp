#include "CapturerTrackSource.h"
#include "ConnecttionFactory.h"
#include <QDebug>
const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";

CapturerTrackSource* CapturerTrackSource::install = NULL;

CapturerTrackSource::CapturerTrackSource()
{
    mCapturerTrackSource = new rtc::RefCountedObject<CapturerTrackSourceBase>();
}

void CapturerTrackSource::CreateVideoTrackAndAudioTrack()
{
    mAudioTrack = ConnecttionFactory::Get()->CreateAudioTrack(kAudioLabel, ConnecttionFactory::Get()->CreateAudioSource(cricket::AudioOptions()));

    if (mCapturerTrackSource){
        mVideoTrack = ConnecttionFactory::Get()->CreateVideoTrack(kVideoLabel, mCapturerTrackSource);
    }
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> CapturerTrackSource::GetVideoTrack()
{
    return mVideoTrack;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> CapturerTrackSource::GetAudioTrack()
{
    return mAudioTrack;
}

CapturerTrackSource *CapturerTrackSource::GetInstall()
{
    if(install == NULL){
        install = new CapturerTrackSource();
        return install;
    }
    return install;
}

CapturerTrackSourceBase::CapturerTrackSourceBase() : VideoTrackSource(false)
{
    const size_t kWidth = 640;
    const size_t kHeight = 480;
    const size_t kFps = 30;
    const size_t kDeviceIndex = 0;
    capturer_ = new VcmCapture(kWidth, kHeight, kFps, kDeviceIndex);
}
