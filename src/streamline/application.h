#ifndef APPLICATION_H
#define APPLICATION_H

#include <defines.h>
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <Qt>

#include "infoWidget.h"
#ifdef RENDER
#include "glwidget.h"
#endif

class HBox : public QHBoxLayout {
	public:
		HBox(QWidget* parent) : QHBoxLayout(parent) {}
		~HBox() {}

	private:
		Qt::Orientations expandingDirections() const override {
			return Qt::Horizontal | Qt::Vertical;
		}
};

class Application : public QMainWindow {
	Q_OBJECT
	public:
		Application();
		~Application();

		// A single timestep
		void step();
		
		

	private:
		QWidget* centralWidget;
#ifdef RENDER
		GLWidget* glWidget;
#endif
		InfoWidget* infoWidget;
		HBox* mainLayout;

		void resizeEvent(QResizeEvent* event);
};

#endif