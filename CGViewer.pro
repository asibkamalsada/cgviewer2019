#-------------------------------------------------
#
# Project created by QtCreator 2014-03-06T11:52:40
#
#-------------------------------------------------

QT          += core gui opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = CGViewer
TEMPLATE = app

OBJECTS_DIR		= ./obj 
MOC_DIR 		= ./moc

QMAKE_CXXFLAGS += -std=c++0x

SOURCES     +=  src/main.cpp\
                src/Mainwindow.cpp \
                src/Scene.cpp \
                src/EditWidgets.cpp \
                src/Model.cpp \
                src/Light.cpp \
                src/Skybox.cpp \
                src/Sphere.cpp

HEADERS     +=  src/Mainwindow.h \
                src/CGTypes.h \
                src/Scene.h \
                src/EditWidgets.h \
                src/Model.h \
                src/Light.h \
                src/CGFunctions.h \
                src/Skybox.h \
                src/Sphere.h

CONFIG      += mobility debug console
MOBILITY    =

OTHER_FILES += \
                    shader/fragment.glsl \
                    shader/fragment2.glsl \
                    shader/vertex.glsl \
                    plane/plane.obj \
                    plane/plane.mtl \
DISTFILES += \

CONFIG += c++11

LIBS += -lopengl32

DISTFILES += \
    shader/fragment_background.glsl \
    shader/fragment_background_lines.glsl \
    shader/fragment_contour.glsl \
    shader/fragment_skybox.glsl \
    shader/fragment_sphere.glsl \
    shader/fragment_stencil.glsl \
    shader/vertex_background.glsl \
    shader/vertex_contour.glsl \
    shader/vertex_skybox.glsl \
    shader/vertex_sphere.glsl \
    shader/vertex_stencil.glsl
