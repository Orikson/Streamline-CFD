#ifndef APPLICATION_H
#define APPLICATION_H

#include <defines.h>
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QMainWindow>

#include "infoWidget.h"
#ifdef RENDER
#include "glwidget.h"
#endif

class Application : public QMainWindow {
	Q_OBJECT
	public:
		Application();
		~Application();

		// A single timestep
		void step();

	private:
#ifdef RENDER
		GLWidget* glWidget;
#endif
		InfoWidget* infoWidget;

};

#endif