#ifndef WEBRTCSTREAM_H
#define WEBRTCSTREAM_H

#include <webrtc/api/media_stream_interface.h>
#include "WebrtcVideoFrame.h"
#include "Common.h"

/*
 * author      : ShifengJin
 * time        : 20191024
 * create time : 20191024
 * change time : 20191024
 * change list :
 *               @20191024: base
 * note : used to webrtc video and audio
 */
class WebrtcStream
{
public:
    WebrtcStream();
    ~WebrtcStream();

    rtc::scoped_refptr<webrtc::VideoTrackInterface> GetVideoTrack();

    rtc::scoped_refptr<webrtc::AudioTrackInterface> GetAudioTrack();

    /*
     * 設置窗口ID
     * @param iWindowID : 設置的窗口ID
     */
    void SetWindowID(WINDOWID iWindowID);

    /*
     * 獲取窗口ID
     * @return 窗口ID
     */
    WINDOWID GetWindowID();

    void SetVideoTrack(rtc::scoped_refptr<webrtc::VideoTrackInterface> iVideoTrack);

    /*
     * 開始渲染,即創建mRender,然後WebrtcVideoFrame類會觸發回調函數OnFrame達到顯示效果
     */
    void StartView();

    /*
     * 關閉渲染,即銷毀mRender
     */
    void StopView();

protected:
    rtc::scoped_refptr<webrtc::VideoTrackInterface> mVideoTrack;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> mAudioTrack;

    WebrtcVideoFrame                               *mRender;
    WINDOWID                                        mWindowID;
};

#endif // WebrtcStream_H
