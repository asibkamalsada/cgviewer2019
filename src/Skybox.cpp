#include "Skybox.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include "Model.h"

using namespace std;

Skybox::Skybox() {
    gl.initializeGLFunctions(QGLContext::currentContext());

    // the position buffer
    positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(&positions[0], positions.size() * sizeof(QVector3D));
    positionBuffer.release();

    // An dieser Stelle wird wieder die nullte Texturebene genutzt.
    gl.glActiveTexture(GL_TEXTURE0);


    QString sky_blue = "skybox/blue/bkg1_";
    QString text = "skybox/text/";
    QString given_example = "skybox/";
    QString black = "skybox/black/";


    QString prefix = given_example;

    skyImages.resize(6);
    skyImages[0] = std::unique_ptr<QImage>(new QImage(QString(prefix + "right.png")));
    skyImages[1] = std::unique_ptr<QImage>(new QImage(QString(prefix + "left.png")));
    skyImages[2] = std::unique_ptr<QImage>(new QImage(QString(prefix + "top.png")));
    skyImages[3] = std::unique_ptr<QImage>(new QImage(QString(prefix + "bot.png")));
    skyImages[4] = std::unique_ptr<QImage>(new QImage(QString(prefix + "back.png")));
    skyImages[5] = std::unique_ptr<QImage>(new QImage(QString(prefix + "front.png")));

    texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);

    texture->create();
    texture->setSize(skyImages[0]->width(),skyImages[0]->height(), skyImages[0]->depth());
    texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture->allocateStorage();
    texture->generateMipMaps();

    texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     (const void*)skyImages[0]->convertToFormat(QImage::Format_RGBA8888).constBits());
    texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     (const void*)skyImages[1]->convertToFormat(QImage::Format_RGBA8888).constBits());
    texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     (const void*)skyImages[2]->convertToFormat(QImage::Format_RGBA8888).mirrored(true, true).constBits());
    texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     (const void*)skyImages[3]->convertToFormat(QImage::Format_RGBA8888).mirrored(true, true).constBits());
    texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     (const void*)skyImages[4]->convertToFormat(QImage::Format_RGBA8888).constBits());
    texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     (const void*)skyImages[5]->convertToFormat(QImage::Format_RGBA8888).constBits());
}



void Skybox::render(std::shared_ptr<QOpenGLShaderProgram> program, QVector3D cameraPosition) {

    program->bind();
    program->enableAttributeArray("position");
    positionBuffer.bind();
    program->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    program->setUniformValue("cameraPosition", cameraPosition);

    bindTexture();

    //program->setUniformValue("textureCube", 0);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glEnable(GL_DEPTH_TEST);

}

void Skybox::bindTexture(){

    texture->bind(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureId());


}
