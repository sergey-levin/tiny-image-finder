/*
Copyright 2023 Sergey Levin <serjth@levz.dev>


This file is part of Image Finder.

Image Finder is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Image Finder is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Image Finder. If not, see <https://www.gnu.org/licenses/gpl-3.0.txt>.
*/

#include "applicationcore.h"

#include "imagecache.h"
#include "logging.h"

#include <QtGlobal>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QTimer>

ApplicationCore::ApplicationCore():
      QObject(),
      mEngine(),
      mSettings(),
      mFinder(mSettings.matchNames()),
      mModel(mEngine)
{
    mEngine.rootContext()->setContextProperty(QStringLiteral("imageFinder"), &mFinder);
    mEngine.rootContext()->setContextProperty(QStringLiteral("imageModel"), &mModel);
    mEngine.rootContext()->setContextProperty(QStringLiteral("settings"), &mSettings);
    mEngine.rootContext()->setContextProperty(QStringLiteral("cache"), new ImageCacheCount(mSettings.previewSize(), this));

    mFinder.setPath(mSettings.path());
    mFinder.setRecursive(mSettings.recursive());
    mFinder.setFilters(mSettings.matchNames());
    connect(&mSettings, &Settings::matchNamesChanged, &mFinder, &ImageFinder::setFilters);
    connect(&mSettings, &Settings::pathChanged, &mFinder, &ImageFinder::setPath);
    connect(&mSettings, &Settings::recursiveChanged, &mFinder, &ImageFinder::setRecursive);
    connect(&mFinder, &ImageFinder::imagesFound, &mModel, &ImageModel::addSources, Qt::QueuedConnection);
    connect(&mEngine, &QQmlApplicationEngine::objectCreated, this, &ApplicationCore::integrateObject);

    mEngine.load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() {
        mFinder.cancel();
        mSettings.save();
        const auto objs = mEngine.rootObjects();
        for (auto o : objs)
            delete o;
    });
}

ApplicationCore::~ApplicationCore()
{
}

QQuickWindow *ApplicationCore::rootObject() const
{
    return qobject_cast<QQuickWindow *>(mEngine.rootObjects().at(0));
}

void ApplicationCore::integrateObject(QObject *object)
{
    if (!object) {
        qCDebug(Log::main) << "Errors when loading the GUI";
        QTimer::singleShot(0, QCoreApplication::instance(), []() {
            QCoreApplication::instance()->exit(1);
        });
        return;
    }

    Q_ASSERT(object == rootObject());

    connect(rootObject(), &QQuickView::sceneGraphError, this, [](QQuickWindow::SceneGraphError /*error*/, const QString &message) {
        qFatal("%s", qPrintable(message));
    });
    connect(object, &QObject::destroyed, qGuiApp, &QCoreApplication::quit);
    object->setParent(&mEngine);
}
