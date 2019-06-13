#ifndef SCENE_H
#define SCENE_H

#include <memory>

#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QTime>
#include <QTimer>
#include <QVector4D>
#include <QWheelEvent>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/QGLWidget>
#include "Skybox.h"
#include "Sphere.h"
#include "Portal.h"

class Model;
struct Material;
class Light;

class Scene : public QGLWidget {
  Q_OBJECT
public:
  Scene(QWidget *parent = 0);
  ~Scene();

  // these functions are called by the GUI, when the user selects the
  // corresponding menu entry
  void scaleModel(double sx, double sy, double sz);
  void translateModel(double dx, double dy, double dz);
  void rotateModel(double ax, double ay, double az, double angle);
  void deleteModel();
  bool loadModelFromOBJFile(QString path);
  void addLight();

  void saveScene(QString filepath);
  void loadScene(QString filepath);

  std::vector<std::shared_ptr<Sphere>> spheres;

protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();

public slots:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void reloadShader();
  void resetScene();
  void setFloor();

  void triangleInit();


protected slots:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:


  const int sphere_texture_width = 1024;

  std::shared_ptr<QOpenGLTexture> texture;

  std::shared_ptr<QOpenGLShaderProgram>
  loadShaders(QString vertexShaderSource, QString fragmentShaderSource);

  // for camera movement
  int xRot;
  int yRot;
  int zRot;
  float zoom;
  float transx;
  float transy;
  QPoint lastPos;
  QVector4D lastDraggingPos;
  float draggingDepth;
  bool isDragging;
  bool mousepressed;
  void normalizeAngle(int *angle);

  QOpenGLVertexArrayObject vao;
  // show the floor
  bool showFloor;


  std::shared_ptr<Skybox> m_skybox;
  std::shared_ptr<Portal> m_portal;


  // the "main" Shader program
  std::shared_ptr<QOpenGLShaderProgram> m_portalProgram, m_contourProgram, m_program, m_program2, m_skyboxProgram, m_sphereProgram, m_whiteProgram, m_blackProgram, m_backgroundProgram2;
  // the transformation matrices
  QMatrix4x4 m_view, m_projection;
  // function to set the matrices according to camera movement
  void setTransformations();

  // the models in the scene
  std::vector<std::shared_ptr<Model>> m_models;

  // the lights in the scene
  std::vector<std::shared_ptr<Light>> m_lights;

  // the selected Model
  int m_selectedModel;

  // the timer to repaint the scene
  QTimer updateTimer;


  std::vector<QVector3D> background_positions = {
      QVector3D{-1,1,0},
      QVector3D{-1,-1,0},
      QVector3D{1,1,0},
      QVector3D{-1,-1,0},
      QVector3D{1,1,0},
      QVector3D{1,-1,0}

  };
  QOpenGLBuffer background_positionBuffer = QOpenGLBuffer{QOpenGLBuffer::VertexBuffer};

  const std::vector<QVector3D> background_positions_12 = {

      QVector3D(-50.0f,50.0f,-50.0f), // triangle 1 : begin
      QVector3D(-50.0f,-50.0f, -50.0f),
      QVector3D(-50.0f, -50.0f, 50.0f), // triangle 1 : end

      QVector3D(-50.0f,50.0f,-50.0f),// 5
      QVector3D(-50.0f, -50.0f, 50.0f),
      QVector3D(-50.0f, 50.0f,50.0f),

      QVector3D(-50.0f,-50.0f,-50.0f),
      QVector3D(-50.0f, 50.0f,-50.0f), // triangle 2 : end
      QVector3D(50.0f, 50.0f,-50.0f), // triangle 2 : begin

      QVector3D(-50.0f,-50.0f,-50.0f),
      QVector3D(50.0f, 50.0f,-50.0f),// 4
      QVector3D(50.0f,-50.0f,-50.0f),

      QVector3D(-50.0f,-50.0f,-50.0f),
      QVector3D(-50.0f,-50.0f,50.0f),
      QVector3D(50.0f,-50.0f, 50.0f), // 3

      QVector3D(-50.0f,-50.0f,-50.0f),
      QVector3D(50.0f,-50.0f, 50.0f),// 6
      QVector3D(50.0f,-50.0f, -50.0f),

      QVector3D(-50.0f,-50.0f, 50.0f),
      QVector3D(-50.0f,50.0f, 50.0f),
      QVector3D( 50.0f, 50.0f, 50.0f),// 7

      QVector3D(-50.0f, -50.0f, 50.0f),// 12
      QVector3D(50.0f, 50.0f, 50.0f),
      QVector3D(50.0f,-50.0f, 50.0f),

      QVector3D(50.0f,50.0f,-50.0f), // triangle 1 : begin
      QVector3D(50.0f,-50.0f, -50.0f),
      QVector3D(50.0f, -50.0f, 50.0f), // triangle 1 : end

      QVector3D(50.0f,50.0f,-50.0f),// 5
      QVector3D(50.0f, -50.0f, 50.0f),
      QVector3D(50.0f, 50.0f,50.0f),

      QVector3D(-50.0f, 50.0f,-50.0f),
      QVector3D(-50.0f, 50.0f, 50.0f),// 10
      QVector3D(50.0f, 50.0f,50.0f),

      QVector3D(-50.0f, 50.0f,-50.0f),
      QVector3D(50.0f, 50.0f, 50.0f),
      QVector3D(50.0f, 50.0f, -50.0f)// 11
  };



  // AUFGABE 1: hier sollten Sie die Attribute setzen und die Buffer deklarieren
/*

  const QVector3D pos_data[3] = {
      QVector3D{-1.0f, -1.0f, 0.0f},
      QVector3D{ 1.0f, -1.0f, 0.0f},
      QVector3D{ 0.0f,  1.0f, 0.0f}
  };

  const QVector3D col_data[3] = {
      QVector3D{0.0f ,1.0f, 0.0f},
      QVector3D{0.0f ,0.0f, 1.0f},
      QVector3D{1.0f ,0.0f, 0.0f}
  };

  QOpenGLBuffer pos_buffer = QOpenGLBuffer{QOpenGLBuffer::VertexBuffer};
  QOpenGLBuffer col_buffer = QOpenGLBuffer{QOpenGLBuffer::VertexBuffer};



  // AUFGABE 1: ENDE
*/



  // to measure the time since the program started (can be used for animation)
  int frame;


  std::shared_ptr<QOpenGLFramebufferObject> frameBufferObject;

  QMatrix4x4 viewMatrix_cube;

  QMatrix4x4 projectionMatrix_cube;

  std::vector<QVector3D> faces_dir;

  std::vector<QOpenGLTexture::CubeMapFace> faces_cube;


};

#endif // SCENE_H
