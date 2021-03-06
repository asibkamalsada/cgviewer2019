#include <cmath>

#include <QMessageBox>
#include <fstream>
#include <algorithm>

#include "CGFunctions.h"
#include "EditWidgets.h"
#include "Light.h"
#include "Model.h"
#include "Scene.h"



inline void OpenGLError() {
  GLenum errCode;
  if ((errCode = glGetError()) != GL_NO_ERROR) {
    qDebug() << "OpenGL Error: \n" << errCode << endl;
  }
}

Scene::Scene(QWidget *parent) : QGLWidget(parent) {
    m_program = nullptr;

    mousepressed = false;
    isDragging = false;

    frame = 0;

    Model::nameCount = 10;
    m_selectedModel = -1;

    showFloor = true;

    // start the update Timer (30fps)
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    updateTimer.start(33);

    OpenGLError();
}

void Scene::resetScene() {
    // delete lights and models
    m_models.clear();
    m_lights.clear();
    m_selectedModel = -1;
    // reset camera
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom = -10.0f;
    transx = 0;
    transy = -2.0;
    // load plane
    loadModelFromOBJFile(QString("plane/plane.obj"));
    m_models.back()->rotate(QVector3D(1.0, 0.0, 0.0), -90);
    m_models.back()->scale(10, 0, 10);
}

Scene::~Scene() {
  if (m_program) {
    m_program = 0;
  }
}

void Scene::saveScene(QString filepath) {
  // create File
  std::ofstream file(filepath.toStdString().c_str());

  // 1. write header
  file << m_models.size() - 1 - m_lights.size() << " " << m_lights.size()
       << "\n";
  file << xRot << " " << yRot << " " << zRot << " "
       << " " << zoom << " " << transx << " " << transy;
  file << "\n";

  // 2. save Models
  for (size_t m = 1; m < m_models.size(); ++m) {
    std::shared_ptr<Light> l = std::dynamic_pointer_cast<Light>(m_models[m]);
    if (l)
      continue;
    file << m_models[m]->getPath().toStdString() << "\n";
    QMatrix4x4 trafo = m_models[m]->getTransformations();
    for (size_t i = 0; i < 4; ++i)
      for (size_t j = 0; j < 4; ++j)
        file << trafo(i, j) << " ";
    file << "\n";
  }
  file << "\n";

  // 3. save lights
  for (size_t l = 0; l < m_lights.size(); ++l) {
    Light *light = m_lights[l].get();
    QVector3D a = light->getAmbient();
    QVector3D d = light->getDiffuse();
    QVector3D s = light->getSpecular();
    file << a.x() << " " << a.y() << " " << a.z() << "\n";
    file << d.x() << " " << d.y() << " " << d.z() << "\n";
    file << s.x() << " " << s.y() << " " << s.z() << "\n";
    QMatrix4x4 trafo = m_lights[l]->getTransformations();
    for (size_t i = 0; i < 4; ++i)
      for (size_t j = 0; j < 4; ++j)
        file << trafo(i, j) << " ";
    file << "\n";
  }
}

void Scene::loadScene(QString filepath) {
  std::ifstream file(filepath.toStdString().c_str());
  size_t nModels, nLights;
  file >> nModels >> nLights;
  file >> xRot >> yRot >> zRot >> zoom >> transx >> transy;
  float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42,
      m43, m44;
  // read models
  for (size_t m = 0; m < nModels; ++m) {
    std::string path;
    file >> path;
    file >> m11 >> m12 >> m13 >> m14 >> m21 >> m22 >> m23 >> m24 >> m31 >>
        m32 >> m33 >> m34 >> m41 >> m42 >> m43 >> m44;
    loadModelFromOBJFile(QString(path.c_str()));
    QMatrix4x4 trafo = QMatrix4x4(m11, m12, m13, m14, m21, m22, m23, m24, m31,
                                  m32, m33, m34, m41, m42, m43, m44);
    m_models.back()->setTransformations(trafo);
  }
  // read lights
  for (size_t l = 0; l < nLights; ++l) {
    double ar, ag, ab, dr, dg, db, sr, sg, sb;
    file >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb;
    file >> m11 >> m12 >> m13 >> m14 >> m21 >> m22 >> m23 >> m24 >> m31 >>
        m32 >> m33 >> m34 >> m41 >> m42 >> m43 >> m44;
    QMatrix4x4 trafo = QMatrix4x4(m11, m12, m13, m14, m21, m22, m23, m24, m31,
                                  m32, m33, m34, m41, m42, m43, m44);
    auto light = std::shared_ptr<Light>(new Light(Model::nameCount++));
    light->setAmbient(ar, ag, ab);
    ;
    light->setDiffuse(dr, dg, db);
    light->setSpecular(sr, sg, sb);
    light->setTransformations(trafo);
    m_lights.push_back(light);
    m_models.push_back(m_lights.back());
  }
}

void Scene::deleteModel() {
  if (m_selectedModel == -1)
    return;

  updateTimer.stop();

  // check, if model is also a light
  std::shared_ptr<Light> l =
      std::dynamic_pointer_cast<Light>(m_models[m_selectedModel]);
  if (l) {
    // delete the corresponding Light
    int name = m_models[m_selectedModel]->getName();
    int lightIndex = -1;
    for (size_t i = 0; i < m_lights.size(); ++i) {
      if (m_lights[i]->getName() == name)
        lightIndex = i;
    }
    if (lightIndex >= 0) {
      std::vector<std::shared_ptr<Light>>::iterator it = m_lights.begin();
      m_lights.erase(it + lightIndex);
    } else
      qDebug() << "Strange error: Did not find light to erase.\n";
  }

  // delete the model
  std::vector<std::shared_ptr<Model>>::iterator it = m_models.begin();
  m_models.erase(it + m_selectedModel);

  // unselect Model
  m_selectedModel = -1;

  updateTimer.start(33);
}

bool Scene::loadModelFromOBJFile(QString path) {
  m_models.push_back(std::shared_ptr<Model>(Model::importOBJFile(path)));
  if (m_models.back()->getNpositions() == 0) {
    QMessageBox::warning(this, QString("Error"),
                         QString("The file could not be opened."));
    return false;
  }
  m_models.back()->rotate(QVector3D(1.0, 0.0, 0.0), -90);
  return true;
}

void Scene::scaleModel(double sx, double sy, double sz) {
  if (m_selectedModel == -1)
    return;
  m_models[m_selectedModel]->scale(sx, sy, sz);
}

void Scene::translateModel(double dx, double dy, double dz) {
  if (m_selectedModel == -1)
    return;
  m_models[m_selectedModel]->translate(dx, dy, dz);
}

void Scene::rotateModel(double ax, double ay, double az, double angle) {
  if (m_selectedModel == -1)
    return;
  m_models[m_selectedModel]->rotate(QVector3D(ax, ay, az), angle);
}

void Scene::addLight() {
  auto l = std::shared_ptr<Light>(new Light(Model::nameCount++));
  LightDialog ld(l.get());
  if (ld.exec()) {
    m_lights.push_back(l);
    m_models.push_back(m_lights.back());
  }
}

std::shared_ptr<QOpenGLShaderProgram>
Scene::loadShaders(QString vertexShaderSource, QString fragmentShaderSource) {
  auto pr = std::make_shared<QOpenGLShaderProgram>();
  pr->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderSource);
  qDebug() << "Compile VertexShader " << vertexShaderSource << ": ";
  qDebug() << pr->log();

  pr->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderSource);
  qDebug() << "Compile FragmentShader " << fragmentShaderSource << ": ";
  qDebug() << pr->log();

  pr->link();
  qDebug() << "Linking Shader Program: ";
  qDebug() << pr->log();

  return pr;
}

void Scene::reloadShader() {
    m_program = loadShaders(QString("shader/vertex.glsl"),
                          QString("shader/fragment.glsl"));
    m_program2 = loadShaders(QString("shader/vertex.glsl"),
                           QString("shader/fragment2.glsl"));
    m_skyboxProgram = loadShaders(QString("shader/vertex_skybox.glsl"),
                                QString("shader/fragment_skybox.glsl"));
    m_blackProgram = loadShaders(QString("shader/vertex.glsl"),
                                    QString("shader/fragment_black.glsl"));
    m_whiteProgram = loadShaders(QString("shader/vertex.glsl"),
                                 QString("shader/fragment_white.glsl"));
    m_backgroundProgram2 = loadShaders(QString("shader/vertex_skybox.glsl"),
                                    QString("shader/fragment_background_lines.glsl"));
    m_sphereProgram = loadShaders(QString("shader/vertex_sphere.glsl"),
                                QString("shader/fragment_sphere.glsl"));
    m_contourProgram = loadShaders(QString("shader/vertex_contour.glsl"),
                                   QString("shader/fragment_black.glsl"));
    m_portalProgram = loadShaders(QString("shader/vertex.glsl"),
                                  QString("shader/fragment_portal.glsl"));
}

void Scene::setFloor() { showFloor = !showFloor; }

void Scene::initializeGL() {

    environmentTexture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);

    environmentTexture->create();
    environmentTexture->setSize(1024.0f,1024.0f);
    environmentTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    environmentTexture->allocateStorage();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    format.setTextureTarget(GL_TEXTURE_2D);

    tempFBO = std::make_shared<QOpenGLFramebufferObject>(1024.0f, 1024.0f, format);

    environmentMappingViewMatrices.resize(6);

    environmentCubeImages.resize(6);

    spheres =  {

        //std::make_shared<Sphere>(QVector3D(0.0, -10.0,-10.0), 1.0),
        std::make_shared<Sphere>(QVector3D( 0.0f, 0.0f, 0.0f ), 6.0),
        //std::make_shared<Sphere>(QVector3D(-17.0, -10.0,-15.0), 3.0),
        //std::make_shared<Sphere>(QVector3D(-10.0, -5.0,-10.0), 4.0)

    };

    m_skybox = std::make_shared<Skybox>();
    m_portal = std::make_shared<Portal>(QVector3D(0, 4, -10), 4);

    vao.create();
    vao.bind();

    //glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    //glEnable(GL_STENCIL_TEST);
    reloadShader();


    resetScene();

    OpenGLError();
    triangleInit();
}

void Scene::triangleInit() {

  // AUFGABE 1: erzeugen Sie hier die Buffer und initialisieren sie alles
  // notwendige
/*
    pos_buffer.create();
    pos_buffer.bind();
    pos_buffer.setUsagePattern( QOpenGLBuffer::StaticDraw);
    pos_buffer.allocate( pos_data, sizeof( pos_data ));


    col_buffer.create();
    col_buffer.bind();
    col_buffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    col_buffer.allocate( col_data, sizeof(col_data));
*/
}


void Scene::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  qreal aspect = qreal(width) / qreal(height ? height : 1);
  //aspect = 1.0f;
  const qreal zNear = 0.1f, zFar = 400.0f, fov = 60.0f, fov_environment = 90.0f;
  m_projection.setToIdentity();
  m_projection.perspective(fov, aspect, zNear, zFar);
  environmentMappingProjectionMatrix.setToIdentity();
  environmentMappingProjectionMatrix.perspective(fov_environment, 1.0f, zNear, zFar);
}

void Scene::mouseMoveEvent(QMouseEvent *event) {
  if (lastPos.x() == -1 || lastPos.y() == -1)
    return;

  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton && m_selectedModel == -1) {
    setXRotation(xRot + 4 * dy);
    setYRotation(yRot + 4 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(xRot + 4 * dy);
    setZRotation(zRot - 4 * dx);
  } else if (event->buttons() & Qt::MidButton) {
    transx += (double)dx * 0.01;
    transy -= (double)dy * 0.01;
  } else if (event->buttons() & Qt::LeftButton &&
             m_selectedModel != -1) // move the selected model
  {
    QMatrix4x4 imvpMatrix = (m_projection * m_view).inverted();
    QVector4D center = m_models[m_selectedModel]->getBoundingBox().center;
    // determine the last dragging position
    if (!isDragging) // init with bounding box
    {
      // project the center to view (so that we look in z direction -> z
      // is now the depth)
      QVector4D centerView = m_view * center;
      double z = centerView.z();
      // calculate the depth buffer value of z
      draggingDepth = 2.0 * (z - 0.1) / (1000.0 - 0.1) - 1.0;
      lastDraggingPos = unprojectScreenCoordinates(
          event->x(), event->y(), draggingDepth, width(), height(), imvpMatrix);
      isDragging = true;
    }
    QVector4D draggingPos = unprojectScreenCoordinates(
        event->x(), event->y(), draggingDepth, width(), height(), imvpMatrix);
    QVector4D translation = draggingPos - lastDraggingPos;
    float factor = fabs(zoom * 10);
    m_models[m_selectedModel]->translate(translation.x() * factor,
                                         translation.y() * factor,
                                         translation.z() * factor);
    lastDraggingPos = draggingPos;
  }

  lastPos = event->pos();
}

void Scene::wheelEvent(QWheelEvent *event) { zoom -= event->delta() * 0.0025; }

void Scene::mousePressEvent(QMouseEvent *event) {
  mousepressed = true;
  lastPos = event->pos();
}

void Scene::mouseDoubleClickEvent(QMouseEvent *event) {
  // intersect ray with the bounding boxes of all models
  // the functions for this technique are defined in CGFunctions.h

  // calculate intersections of ray in world space
  QMatrix4x4 imvpMatrix = (m_projection * m_view).inverted();
  QVector4D eyeRay_n = unprojectScreenCoordinates(
      event->x(), event->y(), -1.0, width(), height(), imvpMatrix);
  QVector4D eyeRay_z = unprojectScreenCoordinates(
      event->x(), event->y(), 1.0, width(), height(), imvpMatrix);
  float tnear, tfar;
  float smallest_t = 1e33;
  int nearestModel = -1;
  for (size_t i = 0; i < m_models.size(); ++i) {
    BoundingBox bb = m_models[i]->getBoundingBox();
    if (intersectBox(eyeRay_n, eyeRay_z - eyeRay_n, bb.bbmin, bb.bbmax, &tnear,
                     &tfar)) {
      if (tnear < smallest_t) {
        smallest_t = fabs(tnear);
        nearestModel = i;
      }
    }
  }
  if (nearestModel >= 0 && (nearestModel != m_selectedModel)) {
    m_selectedModel = nearestModel;
    std::cout << "Model: " << m_models[m_selectedModel]->getName()
              << " was selected\n";
  } else
    m_selectedModel = -1;
}

void Scene::mouseReleaseEvent(QMouseEvent *event) {
  mousepressed = false;
  isDragging = false;
}

void Scene::setXRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != xRot) {
    xRot = angle;
  }
}

void Scene::setYRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != yRot) {
    yRot = angle;
  }
}

void Scene::setZRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != zRot) {
    zRot = angle;
  }
}

void Scene::normalizeAngle(int *angle) {
  while (*angle < 0)
    *angle += 360 * 16;
  while (*angle > 360 * 16)
    *angle -= 360 * 16;
}

void Scene::setTransformations() {
  m_view = QMatrix4x4(); // init with idendity matrix
  // do the translation
  m_view.translate(transx, transy, zoom);
  // do the rotation
  m_view.rotate((xRot / 16.0f), 1.0f, 0.0f, 0.0f);
  m_view.rotate((yRot / 16.0f), 0.0f, 1.0f, 0.0f);
  m_view.rotate((zRot / 16.0f), 0.0f, 0.0f, 1.0f);
}

void Scene::paintGL()
{
    QOpenGLFramebufferObject::bindDefault();

    const int MAX_LIGHTS = 3;

    ++frame;
    if (frame < 0)
        frame = 0;

    /*
     * render the scene to the framebuffer/screen
     */

    //clear the screen

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //glEnable(GL_MULTISAMPLE);

    //set the view matrix
    setTransformations();

    QVector3D cameraPosition = (m_view.inverted() * QVector4D(0.0, 0.0, 0.0, 1.0)).toVector3DAffine();

    //set the light size, default = 1
    int lightsSize = std::max(std::min((int)m_lights.size(), MAX_LIGHTS), 1);

    //creates the light arrays
    QVector3D *lightsAmbientArray = new QVector3D[lightsSize];
    QVector3D *lightsDiffuseArray = new QVector3D[lightsSize];
    QVector3D *lightsSpecularArray = new QVector3D[lightsSize];
    QVector3D *lightsPositionArray = new QVector3D[lightsSize];    
    //default light
    if (m_lights.size() == 0) {
        lightsAmbientArray[0] = QVector3D(1.0, 1.0, 1.0);
        lightsDiffuseArray[0] = QVector3D(1.0, 1.0, 1.0);
        lightsSpecularArray[0] = QVector3D(1.0, 1.0, 1.0);
        lightsPositionArray[0] = cameraPosition;
    } else {
        for (int j = 0; j < lightsSize; j++) {
            lightsAmbientArray[j] = m_lights[j]->getAmbient();
            lightsDiffuseArray[j] = m_lights[j]->getDiffuse();
            lightsSpecularArray[j] = m_lights[j]->getSpecular();
            lightsPositionArray[j] = m_lights[j]->getBoundingBox().center;
        }
    }



    m_skyboxProgram->bind();
    m_skyboxProgram->setUniformValue("viewMatrix", m_view);
    m_skyboxProgram->setUniformValue("projectionMatrix", m_projection);
    m_skyboxProgram->setUniformValue("cameraPosition", cameraPosition);

    m_skybox->render(m_skyboxProgram, cameraPosition);
    m_skyboxProgram->release();

//---------------------------------------------------------------------------------------------------------------------
/*
    m_blackProgram2->bind();
    m_blackProgram2->enableAttributeArray("position");
    background_positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    background_positionBuffer.create();
    background_positionBuffer.bind();
    background_positionBuffer.allocate(&background_positions_12[0], background_positions_12.size() * sizeof(QVector3D));
    m_blackProgram2->setAttributeBuffer("position", GL_FLOAT, 0, 3);
    //std::cout << cameraPosition.x() << "-" << cameraPosition.y() << "-" << cameraPosition.z() << std::endl;
    m_blackProgram2->setUniformValue("cameraPosition", cameraPosition);
    m_blackProgram2->setUniformValue("viewMatrix", m_view);
    m_blackProgram2->setUniformValue("projectionMatrix", m_projection);

    glDisable(GL_DEPTH_TEST);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glEnable(GL_DEPTH_TEST);

    m_blackProgram2->release();
*/
//---------------------------------------------------------------------------------------------------------------------

    m_program->bind();
    m_program->setUniformValue("texture", 0);
    m_program->release();

//---------------------------------------------------------------------------------------------------------------------
// https://en.wikibooks.org/wiki/OpenGL_Programming/Stencil_buffer
//---------------------------------------------------------------------------------------------------------------------


    size_t i;
    for (showFloor ? i=0 : i=1; i < m_models.size(); ++i)
    {
        QMatrix4x4 modelMatrix = m_models[i]->getTransformations();
        //QMatrix4x4 normalMatrix = ((m_view * modelMatrix).inverted()).transposed();

        if (m_selectedModel == i)
        {
            m_program2->bind();

            m_program2->setUniformValue("modelMatrix", modelMatrix);
            m_program2->setUniformValue("viewMatrix", m_view);
            m_program2->setUniformValue("projectionMatrix", m_projection);

            m_models[i]->render(m_program2);
            m_program2->release();
        }
        else
        {

            if(i != 0){
                glEnable(GL_CULL_FACE);

                m_contourProgram->bind();

                m_contourProgram->setUniformValue("modelMatrix", modelMatrix);
                m_contourProgram->setUniformValue("viewMatrix", m_view);
                m_contourProgram->setUniformValue("projectionMatrix", m_projection);

                m_models[i]->render(m_contourProgram);

                m_contourProgram->release();
                glDisable(GL_CULL_FACE);
            }

            m_program->bind();
            // set the matrix pipeline

            //m_program->setUniformValue("normalMatrix", normalMatrix);
            m_program->setUniformValue("modelMatrix", modelMatrix);
            m_program->setUniformValue("viewMatrix", m_view);
            m_program->setUniformValue("projectionMatrix", m_projection);

            m_program->setUniformValueArray("lightsAmbientArray", lightsAmbientArray, lightsSize);
            m_program->setUniformValueArray("lightsDiffuseArray", lightsDiffuseArray, lightsSize);
            m_program->setUniformValueArray("lightsSpecularArray", lightsSpecularArray, lightsSize);
            m_program->setUniformValueArray("lightsPositionArray", lightsPositionArray, lightsSize);
            m_program->setUniformValue("lightsSize", lightsSize);
            m_program->setUniformValue("cameraPosition", cameraPosition);

            m_models[i]->render(m_program);
            m_program->release();


        }
    }


/*
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);
    // draw where stencil's value is 0
    glStencilFunc(GL_EQUAL, 0, 0xFF);

    size_t q;
    for (q = 1; q < m_models.size(); ++q){
        m_contourProgram->bind();

        m_contourProgram->setUniformValue("modelMatrix", m_models[q]->getTransformations());
        m_contourProgram->setUniformValue("viewMatrix", m_view);
        m_contourProgram->setUniformValue("projectionMatrix", m_projection);


        m_models[q]->render(m_contourProgram);

        m_contourProgram->release();
    }


    // draw only where stencil's value is 1
    glStencilFunc(GL_EQUAL, 1, 0xFF);

    glDisable(GL_STENCIL_TEST);


*/

//---------------------------------------------------------------------------------------------------------------------


    int animationTimeInSeconds = 4;
    int refreshRate = 30;

    float velocity = 10.0 / refreshRate; // coordinates per frame
    int animationFrame = abs((frame  % (refreshRate * animationTimeInSeconds)) - (0.5 * refreshRate * animationTimeInSeconds));
    float movementFactor = sin(frame / 20);

    m_sphereProgram->bind();
    m_sphereProgram->setUniformValue("viewMatrix", m_view);
    m_sphereProgram->setUniformValue("projectionMatrix", m_projection);
    m_sphereProgram->setUniformValue("cameraPosition", cameraPosition);
    m_sphereProgram->release();


    m_program->bind();
    m_program->setUniformValue("projectionMatrix", environmentMappingProjectionMatrix);
    m_program->setUniformValue("cameraPosition", cameraPosition);
    m_program->release();


    //m_skybox->bindTexture();
    QVector3D movedCenter;

    for(auto&& sphere: spheres){
        movedCenter = sphere->getCenter() + float(animationFrame) * QVector3D(0, velocity, 0);

        tempFBO->bind();

        for (unsigned int currentSideCounter = 0; currentSideCounter < 6; ++currentSideCounter){

            QMatrix4x4 currentViewMatrix;

            currentViewMatrix.setToIdentity();

            currentViewMatrix.translate(-movedCenter);

            switch (currentSideCounter) {

            case 0:
                currentViewMatrix.rotate(90.0f, 0.0f, 1.0f, 0.0f);
                break;
            case 1:
                currentViewMatrix.rotate(270.0f, 0.0f, 1.0f, 0.0f);
                break;
            case 2:
                currentViewMatrix.rotate(270.0f, 1.0f, 0.0f, 0.0f);
                break;
            case 3:
                currentViewMatrix.rotate(90.0f, 1.0f, 0.0f, 0.0f);
                break;
            case 4:
                currentViewMatrix.rotate(180.0f, 0.0f, 1.0f, 0.0f);
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            m_skyboxProgram->bind();
            m_skyboxProgram->setUniformValue("viewMatrix", currentViewMatrix);
            m_skyboxProgram->setUniformValue("projectionMatrix", environmentMappingProjectionMatrix);

            m_skybox->render(m_skyboxProgram, cameraPosition);
            m_skyboxProgram->release();

            m_program->bind();

            m_program->setUniformValue("viewMatrix", currentViewMatrix);

            int teta;
            for (showFloor ? teta = 0 : teta = 1; teta < m_models.size(); ++teta){

                m_program->setUniformValue("modelMatrix", m_models[teta]->getTransformations());

                m_program->setUniformValueArray("lightsAmbientArray", lightsAmbientArray, lightsSize);
                m_program->setUniformValueArray("lightsDiffuseArray", lightsDiffuseArray, lightsSize);
                m_program->setUniformValueArray("lightsSpecularArray", lightsSpecularArray, lightsSize);
                m_program->setUniformValueArray("lightsPositionArray", lightsPositionArray, lightsSize);
                m_program->setUniformValue("lightsSize", lightsSize);
                m_program->setUniformValue("cameraPosition", movedCenter);

                m_models[teta]->render(m_program);
            }
            m_program->release();


            environmentTexture->setData(0,
                                        0,
                                        QOpenGLTexture::CubeMapFace(QOpenGLTexture::CubeMapPositiveX + currentSideCounter),
                                        QOpenGLTexture::RGBA,
                                        QOpenGLTexture::UInt8,
                                        tempFBO->toImage().convertToFormat(QImage::Format_RGBA8888).constBits());
            environmentTexture->generateMipMaps();


        }

        QOpenGLFramebufferObject::bindDefault();


        m_sphereProgram->bind();
        //m_skyboxProgram->bind();

        environmentTexture->bind(0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTexture->textureId());

        //m_skybox->bindTexture();

        //sphere->render(m_skyboxProgram, movedCenter);
        //m_skyboxProgram->release();

        sphere->render(m_sphereProgram, movedCenter);
        m_sphereProgram->release();
    }
    m_sphereProgram->release();


    /*

    m_portalProgram->bind();
    m_portalProgram->setUniformValue("viewMatrix", m_view);
    m_portalProgram->setUniformValue("projectionMatrix", m_projection);
    m_portalProgram->setUniformValue("modelMatrix", QMatrix4x4());
    m_portalProgram->setUniformValue("distortionFactor", movementFactor);
    m_portalProgram->setUniformValue("frame", frame);

    m_portal->render(m_portalProgram);

    m_portalProgram->release();

    */
}

std::vector<QMatrix4x4> Scene::translateViewMatrices(QVector3D cameraCenter){
    std::vector<QMatrix4x4> returnMatrices(environmentMappingViewMatrices);
    for (auto&& viewMatrix : returnMatrices){
        viewMatrix.translate(-cameraCenter);
    }
    return returnMatrices;
}

