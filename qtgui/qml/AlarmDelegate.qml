import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

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
        onPressAndHold:  {
            alarmlistmodel.currentIndex =index;
            console.log("Alarm pressed : "+index);
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
                color: "white"
                font.pointSize: 24
                horizontalAlignment: Text.AlignHCenter
                font.family: "Material Design Icons"
            }

            onActivated:{
                console.log("Deleting idx: " + index)
                alarmlistmodel.delete_alarm(index);
            }
        }
    }
}
