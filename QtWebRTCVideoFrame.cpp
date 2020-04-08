#include "QtWebRTCVideoFrame.h"

#include <QDebug>

#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/rtc_base/arraysize.h>
#include <libyuv/convert_argb.h>
#include "QVideoRender.h"

QtWebRTCVideoFrame::QtWebRTCVideoFrame(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}

QtWebRTCVideoFrame::~QtWebRTCVideoFrame()
{
    this->mWindowID = -1;
}

void QtWebRTCVideoFrame::OnFrame(const webrtc::VideoFrame &iFrame)
{
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(iFrame.video_frame_buffer()->ToI420());
    if(iFrame.rotation() != webrtc::kVideoRotation_0){
        buffer = webrtc::I420Buffer::Rotate(*buffer, iFrame.rotation());
    }

    QVideoRender *paintWidget = (QVideoRender*)QWidget::find(mWindowID);;

    if(paintWidget == nullptr)
    {
       qDebug() << "paintWidget nullptr,ERROR ,return";
       return;
    }

    int argbBufferSize = iFrame.height() * iFrame.width() * 4;
    unsigned char * argbBuffer = (unsigned char*)malloc(argbBufferSize);

    RTC_CHECK(argbBuffer!=nullptr);

    int ret =  libyuv::I420ToARGB(
                buffer->DataY(), buffer->StrideY(),
                buffer->DataU(), buffer->StrideU(),
                buffer->DataV(), buffer->StrideV(),
                argbBuffer, buffer->width() * 4,
                buffer->width(), buffer->height());

    paintWidget->RenderARGBVideo(argbBuffer, buffer->width(), buffer->height());

    if(argbBuffer != nullptr){
        free(argbBuffer);
        argbBuffer = nullptr;
    }
}

unsigned long QtWebRTCVideoFrame::GetWindowID() const
{
    return this->mWindowID;
}

void QtWebRTCVideoFrame::SetWindowID(unsigned long iWindowID)
{
    this->mWindowID = iWindowID;
}
