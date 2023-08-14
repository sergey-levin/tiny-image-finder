cmake_minimum_required(VERSION 3.5)

if (APPLE)
    set(APP_EXE TinyImageFinder)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15.0)
    set(APP_VERSION 1.0.0)
else()
    set(APP_EXE image-finder)
    set(APP_VERSION 0.1.2)
endif()

set(SUPPORTED_LANGUAGES CXX)
project(${APP_EXE} VERSION ${APP_VERSION} LANGUAGES ${SUPPORTED_LANGUAGES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Quick LinguistTools)

set(PROJECT_SOURCES
        main.cpp
        imagecache.cpp
        imagecache.h
        imagefinder.cpp
        imagefinder.h
        imagemodel.cpp
        imagemodel.h
        imageprovider.cpp
        imageprovider.h
        logging.cpp
        logging.h
        applicationcore.cpp
        applicationcore.h
        settings.cpp
        settings.h
        paths.cpp
        paths.h
)

set(TS_FILES
    i18n/image-finder_ru.ts
    i18n/image-finder_nl.ts
)
set(ALL_RC "${CMAKE_CURRENT_SOURCE_DIR}/resources/resource.qrc")

if(WIN32)
    set(WIN_RC "${CMAKE_CURRENT_SOURCE_DIR}/resources/win32_resource.rc")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_lupdate(${APP_EXE} TS_FILES ${TS_FILES} SOURCES
        ${PROJECT_SOURCES}
        qml/MainWindow.qml
        qml/ImageView.qml
        qml/SettingsView.qml
    )

    qt_add_executable(${APP_EXE}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${WIN_RC}
        ${ALL_RC}
    )
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

    add_executable(${APP_EXE}
        ${PROJECT_SOURCES}
        ${WIN_RC}
        ${ALL_RC}
    )
endif()

target_compile_definitions(${APP_EXE} PRIVATE
    APP_VERSION="${APP_VERSION}"
)

if(APPLE)
    find_library(
        SSURL
        libSecurityScopedURL.dylib
        PATHS ../macos
    )
endif()

target_link_libraries(${APP_EXE} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Concurrent
    ${SSURL}
)

set_target_properties(${APP_EXE} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${APP_EXE})
endif()

if(UNIX AND NOT APPLE)
    set(APP_ID dev.levz.TinyImageFinder)
    install(FILES package/${APP_ID}.desktop DESTINATION share/applications)
    install(FILES package/${APP_ID}.metainfo.xml DESTINATION share/metainfo)
    install(FILES package/app-info/icons/128x128/${APP_ID}.png DESTINATION share/icons/hicolor/128x128/apps)
    install(DIRECTORY package/app-info DESTINATION share)

    install(TARGETS ${APP_EXE}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

