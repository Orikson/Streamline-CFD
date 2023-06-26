#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <defines.h>
#include <QObject>
#include <QWidget>
#include <QTimer>

#define GL_GLEXT_PROTOTYPES
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
#include <QWheelEvent>

#include <stdlib.h>

#include <clw.h>
#include <VPHMCF/vphmcf.h>
#include <LBM/lbm2d.h>
#include <glshader.h>
#include "scenes/scene.h"

#ifdef VPH_2D
#include "scenes/VPHMCF_2D/bubbleScene.h"
#endif
#ifdef D2Q9
#include "scenes/LBM_2D/turbulentScene.h"
#endif
#ifdef SPH_2D
#include "scenes/SPH_2D/damBreakScene.h"
#endif
#ifdef WAV
#include "scenes/WAV/boatScene.h"
#endif

class GLWidget : public QOpenGLWidget {
	Q_OBJECT
	public:
		GLWidget(QWidget* parent);
		~GLWidget();

		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int w, int h) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;
	
	private:
		static const float vertices[12];
		static const unsigned int indices[6];

		QPoint mousePosition;

		int frame;

		QOpenGLVertexArrayObject* VAO;
		GLuint VBO, EBO;

		InteropTexture2D* screenTexture;
		CLContext* clContext;

		Shader* shader;

		QTimer* pTimer;
		
		Scene* scene;

		// Kernel Mandelbrot
		Kernel mandelbrot;

};

#endif