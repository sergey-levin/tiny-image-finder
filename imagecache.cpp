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

#include "imagecache.h"

#include <QCoreApplication>

#if QT_VERSION < 0x060000

uint qHash(const QSize &size) {
    return qHash(qMakePair(size.width(), size.height()));
}

#endif

ImageCache::ImageCache()
    : QObject()
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &ImageCache::deleteLater);
}

ImageCache& ImageCache::instance()
{
    static auto imageCache = new ImageCache;
    return *imageCache;
}

QImage ImageCache::get(const QString &path, const QSize &size) const
{
    auto previews = mImages.value(path);
    return previews.value(size, QImage());
}

void ImageCache::add(const QString &path, const QSize &size, const QImage &image)
{
    auto previews = mImages.value(path);
    previews.insert(size, image);
    mImages.insert(path, previews);
    emit updated(size, path);
}

void ImageCache::reset()
{
    mImages.clear();
    emit reseted();
}

ImageCacheCount::ImageCacheCount(int previewSize, QObject *parent):
    QObject(parent),
    mSize(previewSize, previewSize)
{
    connect(&ImageCache::instance(), &ImageCache::updated, this, [this](const QSize &size, const QString &path) {
        mCount[size].insert(qHash(path));
        emit countChanged();
    });
    connect(&ImageCache::instance(), &ImageCache::reseted, this, &ImageCacheCount::reset);
}

uint ImageCacheCount::count() const
{
    return mCount.value(mSize).count();
}

void ImageCacheCount::setImageSize(const QSize &value)
{
    mSize = value;
    emit countChanged();
}

void ImageCacheCount::reset()
{
    mCount.clear();
    emit countChanged();
}
