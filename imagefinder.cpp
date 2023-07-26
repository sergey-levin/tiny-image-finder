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

#include "imagefinder.h"
#include "logging.h"
#include "paths.h"

#include <QApplication>
#include <QCursor>
#include <QDirIterator>
#include <QImageReader>
#include <QTimer>
#include <QThreadPool>

namespace {

const uint EMIT_RESULTS_MS = 2000;

QStringList findImages(const QString &path, const QStringList &filters)
{
    qCDebug(Log::main) << "Search images in:" << path;
    QDir dir(path);
    const QString canonicalPath = dir.canonicalPath();
    const auto files = dir.entryList(filters, QDir::Files | QDir::Readable, QDir::Name);
    QStringList result;
    std::transform(files.constBegin(), files.constEnd(), std::back_inserter(result), [&canonicalPath](const QString &file) {
        return canonicalPath + "/" + file;
    });
    return result;
}

class Worker: public QRunnable
{
public:
    Worker(UnorderedPaths &todo, UnorderedPaths &done, OrderedPaths &result,
           const QString &path, const QStringList &filters):
        QRunnable(),
        mTodo(todo),
        mDone(done),
        mResult(result),
        mFilters(filters),
        mPath(QDir(path).canonicalPath())
    {}
    virtual void run() {
        const bool processed = mDone.addOne(mPath);
        if (!processed) {
            QDir dir(mPath);
            const QString canonicalPath = dir.canonicalPath();
            const auto names = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            QSet<QString> paths;
            paths.reserve(names.size());
            for (const QString &name : names) {
                if (!name.isEmpty())
                    paths << canonicalPath + "/" + name;
            }
            mTodo.add(paths);
            const auto images = findImages(mPath, mFilters);
            if (!images.isEmpty())
                mResult.add(images);
        }
    }
private:
    UnorderedPaths &mTodo;
    UnorderedPaths &mDone;
    OrderedPaths &mResult;
    const QString mPath;
    const QStringList mFilters;
};

} // namespace

ImageFinder::ImageFinder(const QStringList &filters):
    QObject(),
    mRecursive(true),
    mCancelled(false),
    mPath(),
    mFilters(),
    mTodo(new UnorderedPaths),
    mDone(new UnorderedPaths),
    mResult(new OrderedPaths)
{
#if QT_VERSION >= 0x060000
    QImageReader::setAllocationLimit(512);
#endif
    setFilters(filters);
}

void ImageFinder::cancel()
{
    mCancelled = true;
    QThreadPool::globalInstance()->waitForDone();
}

ImageFinder::~ImageFinder()
{
    cancel();
}

void ImageFinder::setFilters(const QStringList &filters)
{
    mFilters.clear();
    const auto formats = QImageReader::supportedImageFormats();
    if (filters.isEmpty()) {
        std::transform(formats.constBegin(), formats.constEnd(), std::back_inserter(mFilters), [](const QByteArray &format) {
            return QString("*.%1").arg(QString(format));
        });
    } else {
        for (const QString &name : filters) {
            if (name.isEmpty())
                continue;
            std::transform(formats.constBegin(), formats.constEnd(), std::back_inserter(mFilters), [&name](const QByteArray &format) {
                return QString("*%1*.%2").arg(name).arg(QString(format));
            });
        }
    }
    qCDebug(Log::main) << "Image filters:" << mFilters;
}

void ImageFinder::search()
{
    if (mRecursive) {
        QThreadPool::globalInstance()->start([this] {
            QElapsedTimer t;
            t.start();

            QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            mCancelled = false;
            mResult->clear();
            mDone->clear();
            mTodo->clear();
            mTodo->add(QSet<QString>{ mPath });

            QString path;
            QElapsedTimer resultTimer;
            resultTimer.start();

            do {
                path = mTodo->takeOne();
                if (!path.isEmpty())
                    QThreadPool::globalInstance()->start(new Worker(*mTodo, *mDone, *mResult, path, mFilters));
                if (resultTimer.elapsed() > EMIT_RESULTS_MS) {
                    emit imagesFound(mResult->takeAll());
                    resultTimer.restart();
                }
                if (mCancelled)
                    return;
            } while (!path.isEmpty() || QThreadPool::globalInstance()->activeThreadCount() > 1); // 1 - mean this runnable

            emit imagesFound(mResult->takeAll());
            emit finished();

            QGuiApplication::restoreOverrideCursor();
            qDebug() << "Elapsed:" << t.elapsed();
        }, 100);
    } else {
        emit imagesFound(findImages(mPath, mFilters));
        QTimer::singleShot(0, this, [this] { emit finished(); }); // Send signal in queue
    }
}
