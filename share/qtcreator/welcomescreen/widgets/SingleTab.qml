/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

import QtQuick 1.0
import qtcomponents 1.0 as Components

Item {
    id: root
    Components.QStyleItem { cursor: "pointinghandcursor"; anchors.fill: parent }
    height: 22
    property int offset: 0;
    y: offset

    width: 140

    Rectangle {
        color: "#a6a6a6"
        width: 1
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.bottomMargin: 2
        anchors.topMargin: 2
    }
    Rectangle {
        color: "#666666"
        width: 1
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.bottomMargin: 4
        anchors.topMargin: 4
    }

    Rectangle {
        id: item
        width: 40
        height: root.height
        radius: 16
        opacity: 0
        gradient: Gradient {
            GradientStop {
                id: gradientstop1
                position: 0
                color: "#5e5e5e"
            }

            GradientStop {
                id: gradientstop2
                position: 0.890
                color: "#4d4c4c"
            }
        }
        anchors.rightMargin: 12
        anchors.leftMargin: 12
        smooth: true
        border.width: 1
        border.color: "#d1d1d1"
        anchors.verticalCenter: text.verticalCenter
        anchors.right: text.right
        anchors.left: text.left
        Behavior on opacity {
            PropertyAnimation { duration: 50 }
        }
    }

    Text {
        id: text
        horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter
        anchors.fill: parent
        text: model.modelData.title
        elide: Text.ElideRight
        color: "black"
    }
    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: tabBar.current = index
    }
    states: [
        State {
            name: "hover"
            id: hoverState; when: mouseArea.containsMouse && tabBar.current !== index

            PropertyChanges {
                target: item
                opacity: 0.100
            }
        },
        State {
            name: "active"
            id: activeState; when: tabBar.current === index
            PropertyChanges { target: text; color: "#f3f3f3" }
            PropertyChanges { target: root; offset: 0 }
            PropertyChanges { target: bottomLine; visible: false }

            PropertyChanges {
                target: item
                border.color: "#3a3a3a"
                opacity: 1
            }

        }
    ]
}
