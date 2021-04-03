// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

import QtQuick.Extras 1.4

import "Jsutil.js" as Util

Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    modal: true;
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    Timer {
        id: sleepTimeOutMenuCloseTimer;
        interval: applicationWindow.dialogTimeout;
        running: false;
        repeat: false;
        onTriggered: parent.close();
    }

    contentItem: GridLayout {
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.slim;
        rows: 3;
        columns:2;
        columnSpacing: Style.itemSpacings.medium;
        rowSpacing: Style.itemSpacings.dense;

        Text{
            text: "Sleep after minutes";
            font: Style.font.label;
            color: Style.colors.primaryText;
            elide: Text.ElideRight;
            Layout.topMargin: Style.itemMargins.wide;
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        }

        Tumbler{
            id: sleepTimeoutTumbler
            Layout.maximumHeight: 120;
            Layout.preferredHeight: 100;
            Layout.fillHeight: true;
            Layout.rowSpan: 2;
            Layout.margins: Style.itemMargins.medium;
            Layout.alignment: Qt.AlignCenter| Qt.AlignTop

            TumblerColumn {
                id: sleepTimeoutMinutes
                model: [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120];
                width:  64;
                delegate: Text {
                    text: styleData.value;
                    font: Style.font.tumbler;
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignHCenter;
                    anchors.margins: 0;
                    topPadding: 0;
                    bottomPadding: 0;
                    opacity: 0.3 + Math.max(0, 1 - Math.abs(styleData.displacement)) * 0.7
                }
            }
        }

        Button{
            id: sleepTimeoutMenuOK;
            text: "Ok";
            font: Style.font.label;
            Layout.preferredWidth: sleepTimeoutMenuCancel.width;
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            onClicked: {
                var minutes = (sleepTimeoutTumbler.currentIndexAt(0)+1) *10;
                sleeptimer.sleep_timeout_min = minutes;
                sleepTimeoutMenu.close();
            }
        }

        Button{
            id: sleepTimeoutMenuCancel;
            text: "Cancel"
            font: Style.font.label;
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            onClicked: {
                sleepTimeoutMenu.close();
            }
        }

    } // Gridlayout

    onAboutToShow : {
        var idx = (sleeptimer.sleep_timeout_min/10)-1
        sleepTimeoutTumbler.setCurrentIndexAt(0, idx);
        sleepTimeOutMenuCloseTimer.start();
    }

    onAboutToHide : {
        // nothing
    }
}
