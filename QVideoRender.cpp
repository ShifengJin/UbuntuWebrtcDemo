#include "QVideoRender.h"
#include <QDebug>
#include <QPainter>

QVideoRender::QVideoRender(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
}

QVideoRender::~QVideoRender()
{

}

void QVideoRender::RenderARGBVideo(unsigned char *iData, int iWidth, int iHeight)
{
    mMutex.lock();

    const int imageSize = iWidth * iHeight * 4;

    uint8_t *dataTmp = new uint8_t[imageSize];
    try{
        memcpy(dataTmp, iData, imageSize);
    }catch(...){
        delete [] dataTmp;
        return;
    }

    QImage image = QImage(dataTmp, iWidth, iHeight, QImage::Format_ARGB32);
    if(image.isNull()){
        qDebug() << "Receive frame error, width:%d, height:%d" << iWidth << iHeight;
        return;
    }

    mImage = image.copy(0, 0, iWidth, iHeight);

    delete[] dataTmp;

    mMutex.unlock();

    update();
}

void QVideoRender::paintEvent(QPaintEvent *paintEvent)
{
    QPainter painter(this);
    if(!mImage.isNull()){
        paintImage(painter);
    }
}

void QVideoRender::paintImage(QPainter &painter)
{
    mMutex.lock();
    QRect mImageRect = QRect(0, 0, this->width(), this->height());
    painter.drawImage(mImageRect, mImage);
    mMutex.unlock();
}
