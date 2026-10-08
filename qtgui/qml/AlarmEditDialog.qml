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

Popup {
    property Alarm currentAlarm;
    property int index;
    modal: true;
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    contentItem: GridLayout {
        columnSpacing: Style.itemSpacings.dense;
        rowSpacing: 0;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.slim;
        rows: 4;
        columns:2;

        RowLayout {
            id: timeTumbler
            spacing: 2
            Layout.maximumHeight: 100
            Layout.preferredHeight: 100
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            Tumbler {
                id: hoursTumbler
                model: 24
                visibleItemCount: 3
                Layout.preferredWidth: 46
                Layout.fillHeight: true
                delegate: Text {
                    text: modelData < 10 ? "0" + modelData : modelData
                    font: Style.font.tumbler
                    color: Style.colors.primaryText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * 0.6
                }
            }

            Text {
                text: ":"
                font: Style.font.tumbler
                color: Style.colors.primaryText
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Tumbler {
                id: minutesTumbler
                model: 60
                visibleItemCount: 3
                Layout.preferredWidth: 46
                Layout.fillHeight: true
                delegate: Text {
                    text: modelData < 10 ? "0" + modelData : modelData
                    font: Style.font.tumbler
                    color: Style.colors.primaryText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * 0.6
                }
            }
        }

        Switch{
            id: enaAlarm;
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            position: currentAlarm.enabled
            text: currentAlarm.enabled ? qsTr("on") : qsTr("off")
        }

        ComboBox {
            id: period
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            model: ListModel {
                id: model
                ListElement { text: qsTr("Once") }
                ListElement { text: qsTr("Daily") }
                ListElement { text: qsTr("Weekend") }
                ListElement { text: qsTr("Workdays") }
            }
            currentIndex: currentAlarm.period_id;
        }

        ComboBox {
            id: stations
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            Layout.columnSpan: 2

            model: iradiolistmodel
            textRole: "station_name";
        }

        Button{
            id: alarmEditOK;
            text: qsTr("Ok");
            font: Style.font.label;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
            onClicked: {
                // Time from tumbler
                var now = new Date();
                var h_idx = hoursTumbler.currentIndex;
                var m_idx = minutesTumbler.currentIndex;
                now.setHours(h_idx, m_idx, 0);
                currentAlarm.time = now;
                alarmlistmodel.update_row(alarmlistmodel.currentIndex);
                // update station
                currentAlarm.url = iradiolistmodel.get_station_url(stations.currentIndex);
                // alarm period
                currentAlarm.period_id = period.currentIndex;
                // enabled?
                currentAlarm.enabled= enaAlarm.position
                // update list view
                alarmlistmodel.update_row(alarmlistmodel.currentIndex)
                close();
            }
        }

        Button{
            id: alarmEditCancel;
            text: qsTr("Cancel");
            font: Style.font.label;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
            onClicked: {
                close();
            }
        }
    } // Gridlayout

    onAboutToShow : {
        hoursTumbler.currentIndex = Util.get_hours(currentAlarm.time);
        minutesTumbler.currentIndex = Util.get_minutes(currentAlarm.time);

        for (var i=0; i<iradiolistmodel.rowCount() ; i++){
            if(iradiolistmodel.get_station_url(i) === currentAlarm.url){
                stations.currentIndex = i;
                break;
            }
        }
    }
}
