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

#include "settings.h"

#include "logging.h"

#include <QApplication>
#include <QStandardPaths>
#include <QUrl>

#ifdef Q_OS_MACOS
namespace SecurityScopedURL {
    /// macOS security scoped url handlers
    extern bool store(QSettings &s, const QString &key, const QString &filePath, uint maxCount = 1);
    extern QStringList restore(QSettings &s, const QString &key);
    extern void startAccess(QSettings &s, const QString &key, const QString &fileName);
    extern void stopAccess(QSettings &s, const QString &key, const QString &fileName);
} // namespace SecurityScopedURL
#endif

namespace {

static const int WIDTH_DEFAULT = 800;
static const char WIDTH[] = "width";
static const int HEIGHT_DEFAULT = 800;
static const char HEIGHT[] = "height";
static const bool RECURSIVE_DEFAULT = true;
static const char RECURSIVE[] = "recursive";
static const int PREVIEW_SIZE_DEFAULT = 64;
static const char PREVIEW_SIZE[] = "previewSize";
static const char PATH[] = "path";
static const char MATCH_NAMES[] = "matchNames";

static const QRgb BACKGROUND_DEFAULT = qRgb(220, 220, 220); // Gray
static const char BACKGROUND[] = "background";


struct Str {
    const QString PATH_DEFAULT = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0);
    const QStringList MATCH_NAMES_DEFAULT = {};
};

Q_GLOBAL_STATIC(Str, str)

template<typename T>
void set(T &target, const T &value, const std::function<void()> &signal) {
    if (target != value) {
        target = value;
        signal();
    }
}

template<typename T>
void init(Settings &s, const QString &id, T &target, const T &defaultValue) {
    const QVariant v = s.value(id, defaultValue);
    target = v.value<T>();
    if (!s.contains(id))
        s.setValue(id, defaultValue);
}

} // namespace

Settings::Settings():
      QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName())
{
    qCDebug(Log::main) << "Settings file:" << fileName() << QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    init(*this, WIDTH, mWidth, WIDTH_DEFAULT);
    init(*this, HEIGHT, mHeight, HEIGHT_DEFAULT);
    init(*this, RECURSIVE, mRecursive, RECURSIVE_DEFAULT);
    init(*this, PREVIEW_SIZE, mPreviewSize, PREVIEW_SIZE_DEFAULT);
    init(*this, MATCH_NAMES, mMatchNames, str->MATCH_NAMES_DEFAULT);
    init(*this, BACKGROUND, mBackground, BACKGROUND_DEFAULT);
#ifdef Q_OS_OSX
    // MEMO: No default path for macOS (ref. Entitlements.plist)
    const QStringList files = SecurityScopedURL::restore(*this, PATH);
    if (!files.isEmpty())
        mPath = files.first();
    SecurityScopedURL::startAccess(*this, PATH, mPath);
#else
    init(*this, PATH, mPath, str->PATH_DEFAULT);
#endif
}

Settings::~Settings()
{
    save();
}

void Settings::save()
{
    qCDebug(Log::main) << "Saving settings...";
    setValue(WIDTH, mWidth);
    setValue(HEIGHT, mHeight);
    setValue(RECURSIVE, mRecursive);
    setValue(PREVIEW_SIZE, mPreviewSize);
    setValue(MATCH_NAMES, mMatchNames);
    setValue(BACKGROUND, mBackground);
#ifdef Q_OS_MACOS
    SecurityScopedURL::store(*this, PATH, mPath);
#else
    setValue(PATH, mPath);
#endif
    sync();
}

void Settings::setPreviewSize(int value)
{
    set(mPreviewSize, value, [this] { emit previewSizeChanged(); });
}

void Settings::setWidth(int value)
{
    set(mWidth, value, [this] { emit widthChanged(); });
}

void Settings::setHeight(int value)
{
    set(mHeight, value, [this] { emit heightChanged(); });
}

void Settings::setRecursive(bool value)
{
    set(mRecursive, value, [this] { emit recursiveChanged(mRecursive); });
}

void Settings::setBackground(const QColor &value)
{
    set(mBackground, value.rgb(), [this] { emit backgroundChanged(); });
}

void Settings::setPath(const QUrl &url)
{
    if (url.isLocalFile()) {
        const QString value = url.toLocalFile();        
        set(mPath, value, [this, value] {
        #ifdef Q_OS_OSX
            SecurityScopedURL::stopAccess(*this, PATH, value);
            SecurityScopedURL::store(*this, PATH, mPath);
            SecurityScopedURL::startAccess(*this, PATH, mPath);
        #endif
            emit pathChanged(mPath);
        });
    } else {
        qCDebug(Log::main) << "Provided path is not local:" << url;
    }
}

void Settings::setMatchNames(QStringList values)
{
    values.removeAll({});
    values.removeDuplicates();
    set(mMatchNames, values, [this] { emit matchNamesChanged(mMatchNames); });
}
