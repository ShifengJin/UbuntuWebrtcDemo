#ifndef QVIDEORENDER_H
#define QVIDEORENDER_H

#include <QWidget>
#include <QMutex>
#include <QImage>

/*
 * author      : ShifengJin
 * time        : 20191024
 * create time : 20191024
 * change time : 20191024
 * change list :
 *               @20191024: base
 * note : used to draw video frame
 */

class QVideoRender : public QWidget
{
    Q_OBJECT
public:
    explicit QVideoRender(QWidget *parent = nullptr);
    ~QVideoRender();

    void RenderARGBVideo(unsigned char * iData, int iWidth, int iHeight);

private:
    QMutex mMutex;
    QImage mImage;

private:
    void paintEvent(QPaintEvent *paintEvent);
    void paintImage(QPainter &painter);
};

#endif // QVIDEORENDER_H
