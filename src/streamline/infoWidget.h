#ifndef INFO_WIDGET_H
#define INFO_WIDGET_H

#include <defines.h>
#include <QObject>
#include <QWidget>

class InfoWidget : public QWidget {
	Q_OBJECT
	public:
		InfoWidget();
		~InfoWidget();
};

#endif