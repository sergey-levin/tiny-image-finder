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

#include "paths.h"

void OrderedPaths::insert(const QStringList &other)
{
    values << other;
}

void UnorderedPaths::insert(const QSet<QString> &other)
{
    values.unite(other);
}

QString UnorderedPaths::takeOne()
{
    QString result;
    QMutexLocker lock(&m);
    if (!values.isEmpty()) {
        result = *values.constBegin();
        values.erase(values.constBegin());
    }
    return result;
}

bool UnorderedPaths::addOne(const QString &value)
{
    QMutexLocker lock(&m);
    const bool contains = values.contains(value);
    if (!contains)
        values << value;
    return contains;
}
