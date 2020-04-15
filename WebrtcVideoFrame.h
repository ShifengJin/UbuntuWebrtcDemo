#ifndef WEBRTCVIDEOFRAME_H
#define WEBRTCVIDEOFRAME_H

#include <memory>
#include <webrtc/api/media_stream_interface.h>
#include "Common.h"

class WebrtcVideoFrame : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    /* 創建QtWebRTCVideoFrame類
     * @param iWindowID    : 指定顯示窗口的 ID
    */
    WebrtcVideoFrame(WINDOWID iWindowID);

    virtual ~WebrtcVideoFrame();

    /*
     * 獲取圖像後會觸發該回調函數, 可在該函數中實現圖像的渲染
    */
    void OnFrame(const webrtc::VideoFrame& iFrame) override;

    /*
     * 獲取該窗口的WindowID
     * @return WindowID
    */
    WINDOWID GetWindowID() const;

    /*
     * 設置窗口的 WindowID
     * @param iWindowID : 設置的窗口ID
     */
    void SetWindowID(WINDOWID iWindowID);

private:
    WINDOWID mWindowID;
};

#endif // WEBRTCVIDEOFRAME_H
