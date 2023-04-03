// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Page {
    id: settingsPage
    property string objectName : "SettingsPage"

    GridLayout{
        anchors.fill: parent
        columns: 2
        rows: 3
        rowSpacing: Style.itemSpacings.dense;
        columnSpacing:  Style.itemSpacings.dense;
        anchors.margins: Style.itemMargins.medium;

        Text{
            text: "Build: ";
            font: Style.font.label;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        Text{
            text: config.revision + " | " +config.buildtime;
            font: Style.font.valueLabel;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        Text{
            text: "IP: ";
            font: Style.font.label;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        Text{
            text: netinfo.ipaddr;
            font: Style.font.valueLabel;
            color: netinfo.linkstatus? Style.colors.primaryText 
                : Style.colors.disabled;
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        IconButton {
            id: brightnessBtn;
            text: "\uf0de";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                brightnessMenu.popup((applicationWindow.width- brightnessMenu.width)/2,
                                     (applicationWindow.height- brightnessMenu.height)/2)
            }
        }
        Text{
            text: "change brightness";
            font: Style.font.label;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        IconButton {
            id: sleeptimeoutBtn;
            text: "\uf51b";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                sleepTimeoutMenu.popup((applicationWindow.width- sleepTimeoutMenu.width)/2,
                                       (applicationWindow.height- sleepTimeoutMenu.height)/2);
            }
        }
        Text{
            text: "sleep timeout";
            font: Style.font.label;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        IconButton {
            id: wifiBtn;
            text: "\uf5a9";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                wifiMenu.popup((applicationWindow.width- wifiMenu.width)/2,
                               (applicationWindow.height- wifiMenu.height)/2);
            }
        }
        Text{
            text: "Wifi";
            font: Style.font.label;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
    }// GridLayout
}
