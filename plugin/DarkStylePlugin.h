#ifndef DARKSTYLEPLUGIN_H
#define DARKSTYLEPLUGIN_H

//#include "../DarkStyleInterface.h"

#include <QStylePlugin>

//class TraditionalWindowsStyleTreeControl;

QT_BEGIN_NAMESPACE
class QStringList;
class QStyle;
QT_END_NAMESPACE

class DarkStylePlugin : public QStylePlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "darkstyleplugin.json")

public:
  QStringList keys() const;
  QStyle *create(const QString &key) Q_DECL_OVERRIDE;
};

#endif // DARKSTYLEPLUGIN_H
