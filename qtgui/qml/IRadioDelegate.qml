// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

import ruschi.PlayableItem 1.0

Rectangle{
    id: iradiodelegatearea
    width: parent.width;
    height: Style.contentHeight/4;
    radius: 3;
    border.width: 1;
    color: Style.colors.widgetBackground;

    Text {
        id: stationName
        text: station_name;
        elide: Text.ElideRight
        font: Style.font.listItemHead;
        verticalAlignment: Text.AlignVCenter;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.medium;
        color: iradiolist.currentItem === parent ?
            Style.colors.selected : Style.colors.primaryText ;
    }

    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: applicationWindow.pressAndHoldInterval;
        onPressAndHold:{
            iradiolist.currentIndex = index;
            iradiolistmodel.send_to_player(index);
            playerControlWidget.setCurrentMediaTitle(station_name);
            playerControlWidget.show();
        }
    }
}
