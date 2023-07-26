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

#include "applicationcore.h"

#include <QApplication>
#include <QIcon>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator *translator_qt = new QTranslator;
    QTranslator *translator_qtbase = new QTranslator;
    QTranslator *translator_app = new QTranslator;
    const QString name = QLocale::system().name();
    if (!name.startsWith("en_")) {
        const QString lang = name.split("_").value(0);
        Q_UNUSED(translator_qt->load("qt_" + lang, ":/i18n"));
        Q_UNUSED(translator_qtbase->load("qtbase_" + lang, ":/i18n"));
        Q_UNUSED(translator_app->load("image-finder_" + lang, ":/i18n"));
    }
    app.installTranslator(translator_qt);
    app.installTranslator(translator_qtbase);
    app.installTranslator(translator_app);

    app.setApplicationDisplayName("Tiny Image Finder");
    app.setOrganizationName("SerjthWare");
    app.setOrganizationDomain("dev.levz.image-finder"); //MEMO: Should be the same with CFBundleIdentifier in Info.plist
    app.setApplicationVersion(APP_VERSION);
    app.setWindowIcon(QIcon(":/images/logo.png"));

    auto core = new ApplicationCore;
    QObject::connect(&app, &QCoreApplication::aboutToQuit, core, &ApplicationCore::deleteLater);
    return app.exec();
}
