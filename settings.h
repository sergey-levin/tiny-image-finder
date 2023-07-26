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

#include <QColor>
#include <QDir>
#include <QSettings>

/**
 * @brief The Settings class implements application settings
 */
class Settings : public QSettings
{
    Q_OBJECT
    Q_PROPERTY(int previewSize READ previewSize WRITE setPreviewSize NOTIFY previewSizeChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)
    Q_PROPERTY(QStringList matchNames READ matchNames WRITE setMatchNames NOTIFY matchNamesChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
public:
    Settings();
    ~Settings();

    int width() const { return mWidth; }
    int height() const { return mHeight; }
    bool recursive() const { return mRecursive; }
    int previewSize() const { return mPreviewSize; }
    QString path() const { return QDir::toNativeSeparators(mPath); }
    QStringList matchNames() const { return mMatchNames; }
    QColor background() const { return mBackground; }
    void save();

public slots:
    void setWidth(int value);
    void setHeight(int value);
    void setRecursive(bool value);
    void setPreviewSize(int value);
    void setPath(const QUrl &url);
    void setMatchNames(QStringList values);
    void setBackground(const QColor &value);

signals:
    void recursiveChanged(bool);
    void previewSizeChanged();
    void pathChanged(const QString &);
    void matchNamesChanged(const QStringList &);
    void widthChanged();
    void heightChanged();
    void backgroundChanged();

private:
    int mWidth; /// Application window width
    int mHeight; /// Application window height
    bool mRecursive; /// Recursive search
    int mPreviewSize; /// Image preview size
    QString mPath; /// Path to search images
    QStringList mMatchNames; /// Additional image names filters
    QRgb mBackground; /// Previews background color
};
