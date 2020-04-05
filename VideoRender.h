#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QWidget>

#include <QMutex>


class VideoRender : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    VideoRender(QWidget *para);
    ~VideoRender();

    void SetVideoSize(int width, int height);
    void SetYUVData(unsigned char* yuvData);

    void SetViewStated(bool flag);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLShaderProgram *program;

    GLuint textureUniformY, textureUniformU, textureUniformV;
    GLuint idY, idU, idV;

    QOpenGLTexture *pTextureY;
    QOpenGLTexture *pTextureU;
    QOpenGLTexture *pTextureV;

    QOpenGLShader *pVShader;
    QOpenGLShader *pFShader;

    int mVideoW, mVideoH;

    unsigned char* pBufYuv420p;

    bool viewFlag;

    QMutex mMutex;
};

#endif // VIDEORENDER_H
