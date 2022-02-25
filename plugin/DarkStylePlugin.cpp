#include "DarkStyle.h"
#include "DarkStylePlugin.h"
#include "StandardStyle.h"
#include "TraditionalWindowsStyleTreeControl.h"

QStringList DarkStylePlugin::keys() const
{
	return QStringList{"DarkStyle", "StandardStyle"};
}

QStyle *DarkStylePlugin::create(const QString &key)
{
	const QString &lower = key.toLower();
	if (lower == "darkstyle")
		return new DarkStyle;
	if (lower == "standardstyle")
		return new StandardStyle;

	return nullptr;
}
