/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    id: root;
    property alias count: categoryCount.text;
    property alias title: categoryTitle.text
    property QtObject entriesModel;
    property int neededHeight: categoryTitle.height + units.smallSpacing * 2;
    visible: height > 0;
    enabled: visible;
    clip: true;
    MouseArea {
        anchors.fill: parent;
        onClicked: {
            mainWindow.pageStack.push(bookshelf, { focus: true, headerText: "Comics in folder: " + root.title, model: root.entriesModel })
        }
    }
    PlasmaExtras.Title {
        id: categoryTitle;
        anchors {
            margins: units.smallSpacing;
            top: parent.top;
            left: parent.left;
            right: categoryCount.left;
        }
        elide: Text.ElideRight;
    }
    PlasmaComponents.Label {
        id: categoryCount;
        anchors {
            margins: units.smallSpacing;
            verticalCenter: parent.verticalCenter;
            right: parent.right;
        }
        width: paintedWidth;
    }
}