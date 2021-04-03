// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

import ruschi.Alarm 1.0
import "Jsutil.js" as Util

Rectangle{
    id: alarmDelegate
    width: parent.width;
    height: Style.contentHeight/3;
    radius: 3;
    border.width: 1;
    color: alarmEnabled ?
        Style.colors.enabled : Style.colors.disabled;

    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval : applicationWindow.pressAndHoldInterval;
        onPressAndHold:  {
            alarmlistmodel.currentIndex =index;
            alarmEditDlg.index = index;
            alarmEditDlg.currentAlarm = alarmlistmodel.get_alarm(
                alarmlistmodel.currentIndex)
            alarmEditDlg.open();
        }
    }

    RowLayout{
        anchors.fill: parent
        anchors.margins: Style.itemMargins.slim;
        spacing: Style.itemSpacings.medium;

        Text {
            id: periodicityString;
            text: periodstring;
            Layout.fillWidth: true;
            font: Style.font.listItemHead;
        }

        Text {
            id: alarmtime;
            font: Style.font.listItemHead;
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter;
            text: Qt.formatTime(triggerTime, "hh:mm")
        }

        Switch{
            id: enaAlarm;

            position: alarmEnabled;
            text: alarmEnabled ? qsTr("on") : qsTr("off")

            onCheckedChanged:{
                alarmlistmodel.set_enabled(index, position)
            }
        }

        DelayButton{
            Layout.minimumHeight: 56
            Layout.minimumWidth: 56
            Layout.maximumHeight: 56
            Layout.maximumWidth: 56
            delay:1000;

            contentItem: Text{
                text: "\ufa79"
                color: Style.colors.primaryText;
                font.pointSize: 24
                horizontalAlignment: Text.AlignHCenter
                font.family: "Material Design Icons"
            }

            onActivated:{
                alarmlistmodel.delete_alarm(index);
            }
        }
    }
}
