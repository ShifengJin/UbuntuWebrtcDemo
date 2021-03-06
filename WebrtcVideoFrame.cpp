#include "WebrtcVideoFrame.h"

#include <QDebug>

#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/rtc_base/arraysize.h>
#include <libyuv/convert_argb.h>
#include "VideoRender.h"

WebrtcVideoFrame::WebrtcVideoFrame(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}

WebrtcVideoFrame::~WebrtcVideoFrame()
{
    this->mWindowID = -1;
}

void WebrtcVideoFrame::OnFrame(const webrtc::VideoFrame &iFrame)
{
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(iFrame.video_frame_buffer()->ToI420());
    if(iFrame.rotation() != webrtc::kVideoRotation_0){
        buffer = webrtc::I420Buffer::Rotate(*buffer, iFrame.rotation());
    }

    VideoRender *paintWidget = (VideoRender*)QWidget::find(mWindowID);;

    if(paintWidget == nullptr)
    {
       qDebug() << "paintWidget nullptr,ERROR ,return";
       return;
    }

    int yuvBufferSize = buffer->width() * buffer->height() * 3 / 2;
    unsigned char * yuvBuffer = (unsigned char*)malloc(yuvBufferSize);

    RTC_CHECK(yuvBuffer!=nullptr);

    memcpy(yuvBuffer, buffer->DataY(), buffer->width() * buffer->height());
    memcpy(yuvBuffer + buffer->width() * buffer->height(), buffer->DataU(), buffer->width() * buffer->height() / 4);
    memcpy(yuvBuffer + buffer->width() * buffer->height() + buffer->width() * buffer->height() / 4, buffer->DataV(), buffer->width() * buffer->height() / 4);
    paintWidget->SetVideoSize(buffer->width(), buffer->height());
    paintWidget->SetYUVData(yuvBuffer);

    paintWidget->SetViewStated(true);

    if(yuvBuffer != nullptr){
        free(yuvBuffer);
        yuvBuffer = nullptr;
    }
}

unsigned long WebrtcVideoFrame::GetWindowID() const
{
    return this->mWindowID;
}

void WebrtcVideoFrame::SetWindowID(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}
