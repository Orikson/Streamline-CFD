#ifndef INFO_WIDGET_H
#define INFO_WIDGET_H

#include <defines.h>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

class InfoWidget : public QWidget {
	Q_OBJECT
	public:
		InfoWidget(QWidget* parent);
		~InfoWidget();

	private:
		QLabel* title, *subtext;
		QFrame* divider;
		QVBoxLayout* mainLayout;
};

#endif