#include "infoWidget.h"

InfoWidget::InfoWidget(QWidget* parent) : QWidget(parent) {
	// Layout
	mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);
	setMinimumWidth(50);
	setMaximumWidth(200);

	// Labels
	title = new QLabel();
	title->setText("Fluid Options");
	QFont font("Arial", 15);
	title->setFont(font);
	title->setFixedHeight(20);

	subtext = new QLabel();
	subtext->setText("lorem ipsum dolor sit amet");
	subtext->setWordWrap(true);

	// Dividers
	divider = new QFrame();
	divider->setFrameShape(QFrame::HLine);
	divider->setFrameShadow(QFrame::Sunken);

	layout()->addWidget(title);
	layout()->addWidget(divider);
	layout()->addWidget(subtext);
}
InfoWidget::~InfoWidget() {

}