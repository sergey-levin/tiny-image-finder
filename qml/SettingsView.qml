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
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

ColumnLayout {
    anchors.fill: parent
    spacing: 6

    Text {
        Layout.topMargin: 20
        Layout.leftMargin: 100
        Layout.rightMargin: 20
        font.pointSize: 12
        text: qsTr("Options")
    }

    GridLayout {
        width: parent.width
        columns: 2
        Layout.topMargin: 20
        Layout.leftMargin: 20
        Layout.rightMargin: 20

        Text { text: qsTr("Filter:") }

        TextField {
            text: settings.matchNames.join(", ")
            Layout.fillWidth: true
            Layout.preferredWidth: 550
            Layout.alignment: Qt.AlignVCenter
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Comma separated additional image name filters")
            selectByMouse: true
            validator: RegularExpressionValidator { regularExpression: /.+(?:,\s*.+)+$/ }
            onTextChanged: settings.setMatchNames(text.split(/,\s*/))

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                hoverEnabled: true
            }
        }

        Text { text: qsTr("Background:") }

        ToolButton {
            Layout.alignment: Qt.AlignVCenter
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Select background color for Previews")
            hoverEnabled: true
            icon.source: "qrc:///images/color.png"
            icon.color: settings.background
            onClicked: colorDialog.open()
        }

        ColorDialog {
            id: colorDialog
            title: qsTr("Select background color for Previews")
            currentColor: settings.background
            onAccepted: settings.setBackground(colorDialog.color)
        }
    }

    CheckBox {
        height: 50
        Layout.leftMargin: 20
        Layout.rightMargin: 20
        text: qsTr("Recursive search")
        ToolTip.delay: 1000
        ToolTip.timeout: 5000
        ToolTip.visible: hovered
        ToolTip.text: qsTr("Search images in all subfolders of the selected folder")
        checked: settings.recursive
        onCheckedChanged: settings.setRecursive(checked)
    }

    Item { Layout.fillHeight: true }

    Text {
        Layout.leftMargin: 100
        Layout.rightMargin: 20
        font.pointSize: 12
        text: qsTr("Copyright")
    }

    ColumnLayout {
        height: 50
        Layout.margins: 20

        Text { text: "© 2023 " + qsTr("Sergey Levin") }

        RowLayout {
            Text { text: qsTr("Licence: ") }
            ToolButton { text: "GPL v3"; hoverEnabled: true; onClicked: Qt.openUrlExternally("https://www.gnu.org/licenses/gpl-3.0.txt") }
        }
    }

    Text {
        Layout.leftMargin: 100
        Layout.rightMargin: 20
        font.pointSize: 12
        text: qsTr("Author")
    }

    RowLayout {
        height: 50
        Layout.margins: 20

        Rectangle {
            color: "lightsteelblue"; radius: 5
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            Layout.rightMargin: 10
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:///images/personal-logo.png"
            }
        }

        Text { text: qsTr("Sergey Levin") }

        ToolButton {
            icon.source: "qrc:///images/web.png"
            Layout.leftMargin: 50
            Layout.alignment: Qt.AlignVCenter
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: "https://levz.dev/image-finder"
            hoverEnabled: true
            onClicked: Qt.openUrlExternally(ToolTip.text)
        }

        ToolButton {
            property string email: "serjth@levz.dev"
            icon.source: "qrc:///images/email.png"
            Layout.alignment: Qt.AlignVCenter
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Send an email to ") + email
            hoverEnabled: true
            onClicked: Qt.openUrlExternally("mailto:" + email + "?subject=Image Finder")
        }
    }
}
