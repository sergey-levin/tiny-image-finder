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

#include <QHash>
#include <QImage>
#include <QObject>
#include <QSet>

/**
 * @brief The ImageCache class is the singleton used for caching images to fast access
 */
class ImageCache : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageCache)
public:
    static ImageCache& instance();
    QImage get(const QString &path, const QSize &size) const;
    void reset();

public slots:
    void add(const QString &path, const QSize &size, const QImage &image);

signals:
    void updated(const QSize &, const QString &);
    void reseted();

private:
    ImageCache();

    using Previews = QHash<QSize, QImage>;
    QHash<QString, Previews> mImages;
};

/**
 * @brief The ImageCacheCount class implements cached previews counter
 */
class ImageCacheCount : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize imageSize WRITE setImageSize)
    Q_PROPERTY(uint count READ count NOTIFY countChanged)
public:
    ImageCacheCount(int previewSize, QObject *parent = nullptr);
    uint count() const;

public slots:
    void reset();
    void setImageSize(const QSize &value);

signals:
    void countChanged();

private:
    QSize mSize;
    QHash<QSize, QSet<uint>> mCount;
};
