#include "Sphere.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>



Sphere::Sphere(QVector3D m_center, float m_radius)
{
    center = m_center;
    radius = m_radius;
    color  = QVector3D(0.5, 0.1, 0.0);

    QVector3D p1 = QVector3D{2 * radius, 2 * radius, 0}; //oben rechts
    QVector3D p2 = QVector3D{2 * -radius, 2 * radius, 0}; //gegen den Uhrzeigersinn
    QVector3D p3 = QVector3D{2 * -radius, 2 * -radius, 0};
    QVector3D p4 = QVector3D{2 * radius, 2 * -radius, 0};

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

void Sphere::render(std::shared_ptr<QOpenGLShaderProgram> program, QVector3D movedCenter)
{

    program->enableAttributeArray("position");
    positionBuffer.bind();
    program->setAttributeBuffer("position", GL_FLOAT, 0, 3);
    program->setUniformValue("radius", radius);
    //std::cout << movedCenter.x() << " " << movedCenter.y() << " " << movedCenter.z() << std::endl;
    program->setUniformValue("movedCenter", movedCenter);
    program->setUniformValue("color", color);

    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

}


QVector3D Sphere::getCenter(){return center;}

QVector3D Sphere::getNormal(){return normal;}
