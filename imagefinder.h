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

#include <QObject>
#include <QScopedPointer>
#include <QStringList>

class OrderedPaths;
class UnorderedPaths;

/**
 * @brief The ImageFinder class used for recursive multi-threaded finding images in provided path
 */
class ImageFinder final : public QObject
{
    Q_DISABLE_COPY(ImageFinder)
    Q_OBJECT
public:
    explicit ImageFinder(const QStringList &filters);
    ~ImageFinder();

public slots:
    void search();
    void cancel();
    void setRecursive(bool value) { mRecursive = value; }
    void setPath(const QString &value) { mPath = value; }
    void setFilters(const QStringList &filters);

signals:
    void imagesFound(const QStringList&);
    void finished();

private:
    bool mRecursive;
    std::atomic_bool mCancelled;
    QString mPath;
    QStringList mFilters;
    QScopedPointer<UnorderedPaths> mTodo, mDone;
    QScopedPointer<OrderedPaths> mResult;
};

