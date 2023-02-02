#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <defines.h>
#include <QObject>
#include <QWidget>

#define GL_GLEXT_PROTOTYPES
#include <QOpenGLContext>
#include <QOpenGLWidget>

#include <interop.h>

class GLWidget : public QOpenGLWidget {
	Q_OBJECT
	public:
		GLWidget();
		~GLWidget();

		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int w, int h) override;
	
	private:

};

#endif