#ifndef QVIDEORENDER_H
#define QVIDEORENDER_H

#include <QObject>
#include <QWidget>
#include <QMutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


class QVideoRender : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QVideoRender(QWidget *parent = nullptr);
    virtual ~QVideoRender();

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

#endif // QVIDEORENDER_H
