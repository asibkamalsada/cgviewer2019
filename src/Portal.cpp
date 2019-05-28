#include "Portal.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>



Portal::Portal(QVector3D m_center, float m_radius)
{
    gl.initializeGLFunctions(QGLContext::currentContext());
    center = m_center;
    radius = m_radius;
    color  = QVector3D(0.5, 0.1, 0.0);

    QVector3D p1 = QVector3D{ radius,  radius, 0}; //oben rechts
    QVector3D p2 = QVector3D{-radius,  radius, 0}; //gegen den Uhrzeigersinn
    QVector3D p3 = QVector3D{-radius, -radius, 0};
    QVector3D p4 = QVector3D{ radius, -radius, 0};

    sphere_data.resize(6);

    sphere_data[0] = center + p1;
    sphere_data[1] = center + p2;
    sphere_data[2] = center + p3;
    sphere_data[3] = center + p3;
    sphere_data[4] = center + p4;
    sphere_data[5] = center + p1;

    // the position buffer
    positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(&sphere_data[0], sphere_data.size() * sizeof(QVector3D));
    positionBuffer.release();

    gl.glActiveTexture(GL_TEXTURE0);

    std::unique_ptr<QImage> image{new QImage(QString("textures/redfire.jpg"))};

    texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D);

    texture->create();

    texture->setSize(image->width());

    texture->setFormat(QOpenGLTexture::RGBA8_UNorm);

    texture->allocateStorage();

    texture->generateMipMaps();

    texture->setData(0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (const void*)image->convertToFormat(QImage::Format_RGBA8888).constBits());


}

void Portal::render(std::shared_ptr<QOpenGLShaderProgram> program)
{

    program->enableAttributeArray("position");
    positionBuffer.bind();
    program->setAttributeBuffer("position", GL_FLOAT, 0, 3);
    program->setUniformValue("radius", radius);
    program->setUniformValue("center", center);
    program->setUniformValue("color", color);

    texture->bind(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureId());

    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

}


QVector3D Portal::getCenter(){return center;}

QVector3D Portal::getNormal(){return normal;}
