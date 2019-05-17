#include "Sphere.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>



Sphere::Sphere()
{
    center = QVector3D(0.0, 0.0, 0.0);
    radius = 5.0;
    color = QVector3D(0.0, 0.25, 0.75);

    QVector3D p1 = center + QVector3D{radius, radius, 0}; //oben rechts
    QVector3D p2 = center + QVector3D{-radius, radius, 0}; //gegen den Uhrzeigersinn
    QVector3D p3 = center + QVector3D{-radius, -radius, 0};
    QVector3D p4 = center + QVector3D{radius, -radius, 0};

    normal = QVector3D::normal(p3 - p2, p1 - p2);


    std::cout << "normal: " << normal.x() << " , " << normal.y() << " , " << normal.z() << std::endl;

    sphere_data.resize(6);

    sphere_data[0] = p1;
    sphere_data[1] = p2;
    sphere_data[2] = p3;
    sphere_data[3] = p3;
    sphere_data[4] = p4;
    sphere_data[5] = p1;

    // the position buffer
    positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(&sphere_data[0], sphere_data.size() * sizeof(QVector3D));
    positionBuffer.release();



}

void Sphere::render(std::shared_ptr<QOpenGLShaderProgram> program)
{
    program->bind();
    program->enableAttributeArray("position");
    positionBuffer.bind();
    program->setAttributeBuffer("position", GL_FLOAT, 0, 3);
    program->setUniformValue("radius", radius);
    program->setUniformValue("center", center);
    program->setUniformValue("color", color);
    program->setUniformValue("normal", normal);
    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
    program->release();
}


QVector3D Sphere::getCenter(){return center;}

QVector3D Sphere::getNormal(){return normal;}
