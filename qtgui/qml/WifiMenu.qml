// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.4

/**
 * Wifi Menu
 */
Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    dim: true;
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    GridLayout{
        height: wifiMenu.height;
        anchors.leftMargin: Style.itemMargins.slim;
        anchors.rightMargin: Style.itemMargins.slim;
        anchors.topMargin: Style.itemMargins.slim;
        clip: true;
        rows:3;
        columns:2;

        IconButton{
            text: "\uf453";
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
            Layout.leftMargin: Style.itemMargins.wide;
            onPressed : {
                console.log("scan");
                wifictrl.start_scan();
            }
        }

        Text{
            id: wifictrlStatus;
            text: "Press for WPS PBC";
            font: Style.font.label;
            color: "white";
            Layout.rightMargin: Style.itemMargins.wide;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
        }

        WifiList{
            id: wifilistview;
            Layout.fillWidth: true;
            Layout.margins: Style.itemMargins.wide;
            Layout.fillHeight: true;
            Layout.columnSpan: 2;
        }
    }// ColumnLayout
}// Wifi Menu
