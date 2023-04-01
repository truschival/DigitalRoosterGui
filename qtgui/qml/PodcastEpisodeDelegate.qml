// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtMultimedia 5.9

import ruschi.PodcastEpisode 1.0

Rectangle{
    id: rectangle
    width:  stackView.width-2;
    height: 100;
    radius: 3;
    border.width: 1;
    color: Style.colors.widgetBackground;
    anchors.margins: Style.itemMargins.slim;

    GridLayout{
        columns: 2
        rows: 3
        rowSpacing: Style.itemSpacings.dense;
        columnSpacing: Style.itemSpacings.dense;
        anchors.fill: parent
        anchors.margins: Style.itemMargins.slim;
        clip: true

        Text {
            id:episodeDN
            text: title;
            font: listened ?
                Style.font.listItemHeadListened : Style.font.listItemHead;
            Layout.fillWidth: true
            elide: Text.ElideRight
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.bottomMargin: Style.itemMargins.slim;
            color: Style.colors.primaryText;
            maximumLineCount: 2
            wrapMode: Text.WordWrap
        }

        Text {
            id:episodePubDate
            text: pub_date;
            font: listened ?  Style.font.valueLabel : Style.font.label;
            elide: Text.ElideRight
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.topMargin: Style.itemMargins.slim;
            color: Style.colors.primaryText;
        }

        Text {
            id: episodeDuration
            text: duration+"h";
            font: listened ?  Style.font.valueLabel : Style.font.label;
            elide: Text.ElideRight
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.topMargin: Style.itemMargins.slim;
            color: Style.colors.primaryText;
        }
    }
    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: applicationWindow.pressAndHoldInterval;

        onDoubleClicked:{
            episodemodel.currentIndex = index;
        }

        onPressAndHold:  {
            episodemodel.currentIndex = index;
            episodemodel.send_to_player(index)
            playerControlWidget.setCurrentMediaTitle(title)
            playerControlWidget.show()
        }
    }
}
