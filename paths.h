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

#include <QMutex>
#include <QSet>
#include <QStringList>

/**
 * @brief The Paths class implements template for thread-safe container for storing system paths
 */
template <typename T>
class Paths
{
    Q_DISABLE_COPY(Paths)
public:
    Paths() {}
    virtual ~Paths() {}
    qsizetype size() const {
        QMutexLocker lock(&m);
        return values.size();
    }
    void add(const T &other) {
        QMutexLocker lock(&m);
        insert(other);
    }
    void clear() {
        QMutexLocker lock(&m);
        values.clear();
    }
    T takeAll() {
        QMutexLocker lock(&m);
        return std::move(values);
    }

private:
    virtual void insert(const T &other) = 0;

protected:
    mutable QMutex m;
    T values {};
};

/**
 * @brief The OrderedPaths class implements thread-safe container for storing image paths
 */
class OrderedPaths final : public Paths<QStringList>
{
    Q_DISABLE_COPY(OrderedPaths)
public:
    OrderedPaths() {}

private:
    virtual void insert(const QStringList &other);
};

/**
 * @brief The UnorderedPaths class implements thread-safe container for storing directory paths
 */
class UnorderedPaths final : public Paths<QSet<QString>>
{
    Q_DISABLE_COPY(UnorderedPaths)
public:
    UnorderedPaths() {}
    QString takeOne();
    bool addOne(const QString &value);

private:
    virtual void insert(const QSet<QString> &other);
};
