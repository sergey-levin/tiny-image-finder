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

#include "imagemodel.h"

#include "imageprovider.h"
#include "logging.h"

#include <QQmlApplicationEngine>

namespace {

static const int batchSize = 100;

struct Str {
    const QString ID = "asyncModel";
    const QString URL = QString("image://%1/%2").arg(ID);
};

Q_GLOBAL_STATIC(Str, str)

} //namespace

ImageModel::ImageModel(QQmlApplicationEngine &engine, QObject *parent):
      QAbstractListModel(parent),
      mSources()
{
    engine.addImageProvider(str->ID, new ImageProvider);
}

int ImageModel::count() const
{
    return mSources.size();
}

void ImageModel::reset()
{
    beginResetModel();
    mSources.clear();
    endResetModel();
    ImageProvider::resetCache();
    emit sourcesChanged();
}

void ImageModel::addSources(QStringList sources)
{

    // Making sources unique (QSet is not used to keep sources order)
    sources.removeDuplicates();
    const QSet<QString> items {mSources.cbegin(), mSources.cend()};
    QStringList uniqueSources;
    std::copy_if(sources.cbegin(), sources.cend(), std::back_inserter(uniqueSources), [&items](const QString &item) {
        return !items.contains(item);
    });
    if (uniqueSources.isEmpty())
        return;

    const int size = mSources.size();
    beginInsertRows(QModelIndex(), size, size + uniqueSources.size() - 1);
    mSources << uniqueSources;
    endInsertRows();
    emit sourcesChanged();
}

QHash<int, QByteArray> ImageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UrlRole] = "urlPath";
    roles[PathRole] = "filePath";
    return roles;
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mSources.size();
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    const int row = index.row();
    if (row >= mSources.size() || row < 0)
        return {};

    switch (role) {
    case UrlRole:
        return QUrl(str->URL.arg(mSources.value(index.row())));
    case PathRole:
        return mSources.value(index.row());
    default:
        return {};
    }
}
