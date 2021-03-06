#ifndef PORTAL_H
#define PORTAL_H

#include <QGLFunctions>
#include <vector>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QQuaternion>

#include <iostream>
#include <memory>
#include <array>

#include "CGTypes.h"

class Portal
{
public:
    Portal(QVector3D m_center, float m_radius);
    virtual void render(std::shared_ptr<QOpenGLShaderProgram> program);
    QVector3D getCenter();
    QVector3D getNormal();

private:

    std::shared_ptr<QOpenGLTexture> texture;
    QQuaternion rotationQuat;
    QGLFunctions gl;
    QOpenGLBuffer positionBuffer;
    QVector3D center;
    QVector3D normal;
    float radius;
    QVector3D color;
    std::vector<QVector3D> sphere_data
    /*= {

        QVector3D(-1.0f, 1.0f, 0.0f), // triangle 1 : begin
        QVector3D(-1.0f, -1.0f, 0.0f),
        QVector3D(1.0f, -1.0f, 0.0f), // triangle 1 : end

        QVector3D(1.0f, 1.0f, 0.0f), // triangle 2 : begin
        QVector3D(-1.0f, 1.0f, 0.0f),
        QVector3D(1.0f, -1.0f, 0.0f), // triangle 2 : end
    }*/;

};

#endif // PORTAL_H

