#include "QVideoRender.h"
#include <QDebug>
#include <QPainter>

QVideoRender::QVideoRender(QWidget *parent) : QOpenGLWidget(parent)
{
    program = nullptr;

    textureUniformY = 0;
    textureUniformU = 0;
    textureUniformV = 0;

    idY = 0;
    idU = 0;
    idV = 0;

    pTextureY = nullptr;
    pTextureU = nullptr;
    pTextureV = nullptr;

    pVShader = nullptr;
    pFShader = nullptr;

    mVideoW = 0;
    mVideoH = 0;

    pBufYuv420p = nullptr;

    viewFlag = false;

    this->show();
}

QVideoRender::~QVideoRender()
{
    if(pBufYuv420p){
        delete[] pBufYuv420p;
        pBufYuv420p = nullptr;
    }


    if(program){
        program->removeShader(pVShader);
        program->removeShader(pFShader);
        program->release();
        delete program;
        program = nullptr;
    }

    if(pVShader){
        delete pVShader;
        pVShader = nullptr;
    }

    if(pFShader){
        delete pFShader;
        pFShader = nullptr;
    }

    if(pTextureY){
        pTextureY->release();
    }
    if(pTextureU){
        pTextureU->release();
    }
    if(pTextureV){
        pTextureV->release();
    }
}

void QVideoRender::SetVideoSize(int width, int height)
{
    if(pBufYuv420p == nullptr){
        mVideoW = width;
        mVideoH = height;
        pBufYuv420p = new unsigned char[mVideoW * mVideoH * 3 / 2];
    }
}

void QVideoRender::SetYUVData(unsigned char *yuvData)
{
    mMutex.lock();
    memcpy(pBufYuv420p, yuvData, mVideoW * mVideoH * 3 / 2);
    mMutex.unlock();
}

void QVideoRender::SetViewStated(bool flag)
{
    viewFlag = flag;
    if(viewFlag){
        update();
    }
}

void QVideoRender::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    const char *vsrc =
             "attribute vec4 vertexIn;"
             "attribute vec2 textureIn;"
             "varying vec2 textureOut;"
             "void main(void){         "
             "    gl_Position = vertexIn;"
             "    textureOut = textureIn;"
             "}";

    const char *fsrc =
            "varying vec2 textureOut;"
            "uniform sampler2D tex_y;"
            "uniform sampler2D tex_u;"
            "uniform sampler2D tex_v;"
            "void main(void){"
            "    vec3 yuv;"
            "    vec3 rgb;"
            "    yuv.x = texture2D(tex_y, textureOut).r;"
            "    yuv.y = texture2D(tex_u, textureOut).r - 0.5;"
            "    yuv.z = texture2D(tex_v, textureOut).r - 0.5;"
            "    rgb = mat3(1,         1,         1,     "
            "               0,        -0.39465,   2.03211,"
            "               1.13983,  -0.58060,   0) * yuv;"
            "    gl_FragColor = vec4(rgb, 1);"
            "}";

    pVShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    pFShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    bool bCompile = pVShader->compileSourceCode(vsrc);
    if(!bCompile){
        qDebug() << "compileSourceCode vsrc failed!\n" << vsrc;
    }

    bCompile = pFShader->compileSourceCode(fsrc);
    if(!bCompile){
        qDebug() << "compileSourceCode fsrc failed!\n" << fsrc;
    }

    program = new QOpenGLShaderProgram();
    program->addShader(pVShader);
    program->addShader(pFShader);

    program->bindAttributeLocation("vertexIn", 0);
    program->bindAttributeLocation("textureIn", 1);

    program->link();
    program->bind();

    program->enableAttributeArray(0);
    program->enableAttributeArray(1);

    program->setAttributeBuffer(0, GL_FLOAT, 0, 2);
    program->setAttributeBuffer(1, GL_FLOAT, 0, 2);

    textureUniformY = program->uniformLocation("tex_y");
    textureUniformU = program->uniformLocation("tex_u");
    textureUniformV = program->uniformLocation("tex_v");

    static const GLfloat vertexVertices[] = {
        -1.f,    -1.f,
         1.f,    -1.f,
        -1.f,     1.f,
         1.f,     1.f
    };

    static const GLfloat textureVertices[] = {
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 0.f
    };

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertexVertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    pTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    pTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    pTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);

    pTextureY->create();
    pTextureU->create();
    pTextureV->create();

    idY = pTextureY->textureId();
    idU = pTextureU->textureId();
    idV = pTextureV->textureId();

    glClearColor(0.f, 0.f, 0.f, 0.f);
}

void QVideoRender::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void QVideoRender::paintGL()
{
    if(!viewFlag){
        return;
    }

    mMutex.lock();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, idY);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW, mVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, pBufYuv420p);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, idU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW / 2, mVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, pBufYuv420p + mVideoW * mVideoH);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, idV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW / 2, mVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, pBufYuv420p + mVideoW * mVideoH * 5 / 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glUniform1i(textureUniformY, 0);
    glUniform1i(textureUniformU, 1);
    glUniform1i(textureUniformV, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mMutex.unlock();

}

