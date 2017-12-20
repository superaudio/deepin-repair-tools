#include "pluginsloader.h"
#include "repairtoolsinterface.h"

#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QApplication>

PluginsLoader::PluginsLoader(QObject *parent)
    : QObject(parent)
{

}

void PluginsLoader::load()
{
    QDir pluginsDir(qApp->applicationDirPath());

#ifdef QT_DEBUG
    pluginsDir.cd("plugins");
#else
    const QDir pluginsDir("../lib/deepin-repair-tools/plugins");
#endif
    const QStringList plugins = pluginsDir.entryList(QDir::Files);

    for (const QString &file : plugins)
    {
        if (!QLibrary::isLibrary(file))
            continue;

        const QString &filePath = pluginsDir.absoluteFilePath(file);
        QPluginLoader loader(filePath, this);
        qDebug() << "plugin founded:" << filePath << loader.metaData();

        RepairToolsInterface *pluginInter = qobject_cast<RepairToolsInterface *>(loader.instance());
        if (!pluginInter)
        {
            qWarning() << loader.errorString();
            loader.unload();
            continue;
        }

        emit pluginLoaded(pluginInter);
    }

    emit finished();
}
