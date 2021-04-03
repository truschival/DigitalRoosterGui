// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

import ruschi.Alarm 1.0

ListView {
    id:alarmlist
    property string objectName : "Alarms"
    width: stackView.width;
    height: stackView.height;
    contentWidth: stackView.width;
    rebound: listBoundTransition;

    delegate: AlarmDelegate{
        id: alarmdelegate
    }

    AlarmEditDialog{
        id: alarmEditDlg;
        width: Style.contentWidth*0.8;
        x: Math.round((applicationWindow.width - width)/2)
        bottomMargin:15;
    }
    model: alarmlistmodel

    RoundButton {
        text: qsTr("+")
        highlighted: true
        width: 56;
        height: 56;
        anchors.margins: 10;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        onClicked: {
            alarmEditDlg.currentAlarm = alarmlistmodel.create_alarm();
            alarmlist.currentIndex = alarmlistmodel.rowCount()-1;
            alarmEditDlg.open();
        }
    }
}
