// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

/**
 * Power (-Off) Menu
 */
Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    dim: true;
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    Timer {
        id: powerMenuCloseTimer;
        interval: applicationWindow.dialogTimeout;
        running: false;
        repeat: false;
        onTriggered: parent.close();
    }

    onAboutToShow : {
        powerMenuCloseTimer.start();
    }

    GridLayout {
        rows: 2;
        columns: 2;
        rowSpacing: Style.itemSpacings.medium;
        columnSpacing: Style.itemSpacings.medium;

        IconButton {
            id: poweroffBtn
            text: "\uf901"
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                console.log("power off button")
                powerControl.power_off();
            }
        }
        Text{
            text: "shutdown";
            font: Style.font.boldLabel;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }


        IconButton {
            id: rebootBtn
            text: "\uf900";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                console.log("reboot button")
                powerControl.reboot();
            }
        }
        Text{
            text: "reboot";
            font: Style.font.boldLabel;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
    }
}
