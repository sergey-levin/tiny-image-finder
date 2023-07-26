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

#include "imageprovider.h"

#include "logging.h"
#include "imagecache.h"

// Ref. https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quick/imageresponseprovider?h=6.3

namespace {

class AsyncImageResponseRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    AsyncImageResponseRunnable(const QString &id, const QSize &requestedSize)
        : mId(id),
          mRequestedSize(requestedSize)
    {
        setAutoDelete(true);
    }

    void run() override
    {
        QImage image(mId);
        if (image.isNull()) {
            qCDebug(Log::main) << "Unable to open image:" << mId;
            image.load(QString(":/images/error%1x%2.png").arg(mRequestedSize.width()).arg(mRequestedSize.height()));
        } else {
            image = image.scaled(mRequestedSize, Qt::KeepAspectRatio);
        }
        emit done(image);
    }

signals:
    void done(QImage image);

private:
    QString mId;
    QSize mRequestedSize;
};

class AsyncImageResponse : public QQuickImageResponse
{
    Q_OBJECT
public:
    AsyncImageResponse(const QString &id, const QSize &requestedSize, QThreadPool *pool):
          mId(id),
          mRequestedSize(requestedSize)
    {
        auto runnable = new AsyncImageResponseRunnable(id, requestedSize);
        connect(runnable, &AsyncImageResponseRunnable::done, this, &AsyncImageResponse::handleDone);
        pool->start(runnable);
    }

    AsyncImageResponse(const QImage &image):
          mImage(image)
    {
        emit finished();
    }

    void handleDone(QImage image) {
        mImage = image;
        emit finished();
        emit imageLoaded(mId, mRequestedSize, mImage);
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(mImage);
    }

signals:
    void imageLoaded(const QString &, const QSize &, const QImage &);

private:
    QString mId;
    QSize mRequestedSize;
    QImage mImage;
};

} // namespace

QQuickImageResponse *ImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    auto image = ImageCache::instance().get(id, requestedSize);
    AsyncImageResponse *response = nullptr;
    if (image.isNull()) {
        response = new AsyncImageResponse(id, requestedSize, &mPool);
        QObject::connect(response, &AsyncImageResponse::imageLoaded, &ImageCache::instance(), &ImageCache::add);
    } else {
        response = new AsyncImageResponse(image);
    }
    return response;
}

void ImageProvider::resetCache()
{
    ImageCache::instance().reset();
}

#include "imageprovider.moc"
