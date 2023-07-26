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

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14
import Qt.labs.platform 1.1

ApplicationWindow
{
    id: window
    visible: true
    minimumHeight: 500
    minimumWidth: 700
    width: settings.width
    height: settings.height
    title: Qt.application.name + " " + Qt.application.version
    property int imageReady: 0
    property int selectStart
    property int selectEnd
    property int curPos
    property bool settingsVisible: false
    property bool searchButtonPressed: false
    onWidthChanged: settings.setWidth(width)
    onHeightChanged: settings.setHeight(height)

    function search() {
        searchButtonPressed = false;
        imageReady = 0
        imageModel.reset()
        imageFinder.search()
    }

    header: ToolBar {
        height: 45

        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: qsTr("Find")
                Layout.leftMargin: 2
                Layout.rightMargin: 40
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredWidth: 80
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Find images in selected path")
                hoverEnabled: true
                onClicked: {
                    if (settingsVisible) {
                        settingsVisible = false
                        pageLoader.source = "qrc:///qml/ImageView.qml"
                    }
                    if (settings.path === "") {
                        searchButtonPressed = true;
                        folderDialog.open();
                    } else {
                        search()
                    }
                }
            }

            Text {
                text: Qt.platform.os === "osx"
                    ? qsTr("Path:") + " " + settings.path
                    : qsTr("Path:")
                color: "gray"
                elide: Text.ElideMiddle
                Layout.fillWidth: Qt.platform.os === "osx"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            TextField {
                id: textInput
                text: { text = settings.path }
                visible: Qt.platform.os !== "osx"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: text === ""
                              ? qsTr("Path to find images")
                              : qsTr("Selected path to find images: ") + text
                selectByMouse: true
                onTextChanged: settings.setPath("file:" + text)

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    hoverEnabled: true
                    onClicked: {
                        selectStart = textInput.selectionStart;
                        selectEnd = textInput.selectionEnd;
                        curPos = textInput.cursorPosition;
                        pathContextMenu.open();
                        textInput.cursorPosition = curPos;
                        textInput.select(selectStart,selectEnd);
                    }

                    Menu {
                        id: pathContextMenu
                        MenuItem {
                            text: qsTr("Cut")
                            shortcut: "Ctrl+X"
                            onTriggered: textInput.cut()
                        }
                        MenuItem {
                            text: qsTr("Copy")
                            shortcut: "Ctrl+C"
                            onTriggered: textInput.copy()
                        }
                        MenuItem {
                            text: qsTr("Paste")
                            shortcut: "Ctrl+V"
                            onTriggered: textInput.paste()
                        }
                    }
                }
            }

            ToolButton {
                icon.source: "qrc:///images/select-folder.png"
                Layout.rightMargin: 2
                Layout.alignment: Qt.AlignVCenter
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Select folder to find images")
                hoverEnabled: true
                onClicked: folderDialog.open()
            }
        }
    }

    footer: ToolBar {
        height: 45

        RowLayout {
            anchors.fill: parent
            spacing: 6

            Text {
                Layout.leftMargin: 5
                Layout.preferredWidth: 120
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                text: qsTr("Total: ") + imageModel.count
                color: "gray"
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: qsTr("Total images found")
                ToolTip.visible: ma1.containsMouse
                MouseArea {
                    id: ma1
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            Text {
                Layout.preferredWidth: 120
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                text: qsTr("Previews: ") + cache.count
                color: "gray"
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: qsTr("Loaded previews count")
                ToolTip.visible: ma2.containsMouse
                MouseArea {
                    id: ma2
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            Text {
                Layout.preferredWidth: 120
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                text: settings.matchNames.length ? qsTr("Filters: Enabled") : qsTr("Filters: Disabled")
                color: "gray"
            }

            Item {
                Layout.fillWidth: true
            }

            Slider {
                id: imageScaleSlider
                focus: true
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                snapMode: Slider.SnapAlways
                stepSize: 1
                from: 0
                to: 3
                onValueChanged: {
                    settings.setPreviewSize(32 * Math.pow(2, value))
                    cache.setImageSize(Qt.size(settings.previewSize, settings.previewSize))
                }
            }

            Text {
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 50
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                text: settings.previewSize + "x" + settings.previewSize
                color: "gray"
                onTextChanged: {
                    imageReady = 0
                    var v = Math.log2(settings.previewSize / 32)
                    if (v !== imageScaleSlider.value)
                        imageScaleSlider.value = v
                }
            }

            ToolButton {
                Layout.rightMargin: 2
                hoverEnabled: true
                icon.source: settingsVisible
                             ? "qrc:///images/back.png"
                             : "qrc:///images/settings.png"
                Layout.leftMargin: 6
                Layout.alignment: Qt.AlignVCenter
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: settingsVisible
                              ? qsTr("Back to Previews")
                              : qsTr("Open application settings")
                onClicked: {
                    if (settingsVisible) {
                        pageLoader.source = "qrc:///qml/ImageView.qml"
                    } else {
                        pageLoader.source = "qrc:///qml/SettingsView.qml"
                    }
                    settingsVisible = !settingsVisible
                }
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        source: "qrc:///qml/ImageView.qml"
    }

    Popup {
        id: popupMessage
        anchors.centerIn: parent
        background: Rectangle { color: "transparent" }
        contentItem: Rectangle {
            radius: 5
            Text {
                rightPadding: 100
                leftPadding: 100
                topPadding: 10
                bottomPadding: 10
                text: qsTr("No images found!")
            }
        }
        enter: Transition { NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 } }
        exit: Transition { NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 } }
        onAboutToShow: timeout.start()

        Timer {
            id: timeout
            interval: 5000
            onTriggered: popupMessage.close()
        }
    }

    FolderDialog {
        id: folderDialog
        options: FolderDialog.ShowDirsOnly | FolderDialog.ReadOnly
        title: qsTr("Select folder to find images")
        onAccepted: {
            settings.setPath(folderDialog.folder)
            textInput.text = settings.path
            if (searchButtonPressed)
                search()
        }
    }

    Connections {
        target: imageFinder
        function onFinished() {
            if (imageModel.count === 0)
                popupMessage.open()
        }
    }
}
