#include "MainWindow.h"
#include "MainWindow2.h"
#include "../DarkStyleInterface.h"
#include <QApplication>
#include <QPluginLoader>
#include <QProxyStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#if QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QApplication a(argc, argv);


	MainWindow w;
	a.setStyle(QStyleFactory::create("DarkStyle"));
	w.show();

	return a.exec();
}
