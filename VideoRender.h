#ifndef QVIDEORENDER_H
#define QVIDEORENDER_H

#include <QObject>
#include <QWidget>
#include <QMutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


class VideoRender : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit VideoRender(QWidget *parent = nullptr);
    virtual ~VideoRender();

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

    int mMaxVideoW = 1920;
    int mMaxVideoH = 1280;

    unsigned char* pBufYuv420p;

    bool viewFlag;

    QMutex mMutex;
};

#endif // QVIDEORENDER_H
