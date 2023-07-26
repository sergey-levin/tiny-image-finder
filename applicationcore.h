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

#pragma once

#include "imagefinder.h"
#include "imagemodel.h"
#include "settings.h"

#include <QQmlApplicationEngine>

class QQuickWindow;

/**
 * @brief The ApplicationCore class is the main application class
 */
class ApplicationCore : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationCore();
    ~ApplicationCore();
    QQuickWindow* rootObject() const;

private:
    void integrateObject(QObject *object);

    QQmlApplicationEngine mEngine;
    Settings mSettings;
    ImageFinder mFinder;
    ImageModel mModel;
};
