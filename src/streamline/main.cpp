#include <QDebug>
#include <QApplication>
#include <clw.h>

int main() {
	vector<CLDevice> devices = getDevices();
	CLContext* tmp = new CLContext(devices, false);

	delete tmp;
}
