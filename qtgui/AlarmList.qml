import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import ruschi.Alarm 1.0

ListView {
    id:alarmlist
	property string objectName : "Alarms"
    width: stackView.width
    height: stackView.height
    antialiasing: true
    highlightRangeMode: ListView.ApplyRange
    boundsBehavior: Flickable.StopAtBounds
    maximumFlickVelocity: 1500
    flickDeceleration: 1800
    snapMode: ListView.SnapOneItem
    contentWidth: stackView.width
    focus: true

    delegate: AlarmDelegate{
        id: alarmdelegate
    }

	AlarmEditDialog{
		id: alarmEditDlg;
		x: Math.round((applicationWindow.width - width)/2)
		y: Math.round((applicationWindow.height - height)/3)
	}
    model: alarmlistmodel

    RoundButton {
        text: qsTr("+")
        highlighted: true
		width: 56;
		height: 56;
        anchors.margins: 10
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onClicked: {
            alarmEditDlg.currentAlarm = alarmlistmodel.create_alarm();
            alarmlist.currentIndex = alarmlistmodel.rowCount()-1;
            alarmEditDlg.open();
        }
    }
}
