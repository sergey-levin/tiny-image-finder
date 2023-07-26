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

import QtQuick 2.15
import QtQuick.Controls 2.14
import QtQml.Models 2.15
import Qt.labs.platform 1.1

Rectangle {
    id: previews
    color: settings.background

    GridView {
        id: gridView
        clip: true
        anchors.fill: parent
        cellWidth: settings.previewSize + 3 * (imageScaleSlider.value + 1)
        cellHeight: settings.previewSize + 3 * (imageScaleSlider.value + 1)
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true

        model: imageModel

        ScrollBar.vertical: ScrollBar {}

        delegate: Item {
            id: delegateItem
            required property url urlPath
            required property string filePath
            required property int index
            width: gridView.cellWidth
            height: gridView.cellHeight
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.text: qsTr("Location: ") + filePath
            ToolTip.visible: mouseArea.containsMouse

            Image {
                visible: sourceImage.status != Image.Ready
                anchors.fill: parent
                source: "qrc:///images/loading" + settings.previewSize + "x" + settings.previewSize + ".png"
            }

            Image {
                id: sourceImage
                asynchronous: true
                visible: status === Image.Ready
                anchors.centerIn: parent
                sourceSize.width: settings.previewSize
                sourceSize.height: settings.previewSize
                source: urlPath
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.RightButton | Qt.LeftButton
                onClicked: (mouse)=> {
                               if (mouse.button === Qt.LeftButton)
                               gridView.currentIndex = index
                               if (mouse.button === Qt.RightButton && gridView.currentIndex === index)
                               delegateContextMenu.open()
                           }
            }
        }

        Menu {
            id: delegateContextMenu
            MenuItem {
                property Item item: gridView.currentItem
                text: qsTr("Open in External Viewer")
                shortcut: "Ctrl+O"
                onTriggered: Qt.openUrlExternally("file:" + item.filePath)
            }
            MenuItem {
                property Item item: gridView.currentItem
                text: qsTr("Show Containing Folder")
                shortcut: "Ctrl+Shift+O"
                onTriggered: Qt.openUrlExternally("file:" + item.filePath.slice(0, item.filePath.lastIndexOf("/")))
            }
        }
    }
}
