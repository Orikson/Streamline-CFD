#include "application.h"

Application::Application() {
	setWindowTitle("Streamline CFD");
	setMinimumSize(500, 200);

	// Set central widget
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);
	
	// Window layout
	mainLayout = new HBox(centralWidget);

	glWidget = new GLWidget(this);
	infoWidget = new InfoWidget(this);
	mainLayout->addWidget(glWidget);
	mainLayout->addWidget(infoWidget);
}

Application::~Application() {
	
}

void Application::step() {

}

void Application::resizeEvent(QResizeEvent* event) {
	QMainWindow::resizeEvent(event);

	glWidget->setMinimumWidth(event->size().width() * 0.7);
}