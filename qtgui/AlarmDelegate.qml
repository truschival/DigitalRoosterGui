import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

Rectangle{
    id: alarmDelegate
	width: parent.width;
    height: 80;
    radius: 3;
	border.width: 1;
	color: "Grey";
	
	ComboBox {
		id: period
		anchors.top: parent.top;
		anchors.topMargin: 3;
	
		model: ListModel {
			id: model
			ListElement { text: qsTr("Once") }
			ListElement { text: qsTr("Daily") }
			ListElement { text: qsTr("Weekend") }
			ListElement { text: qsTr("Workdays") }
		}
		currentIndex: periodicity;
	}

	Text {
		id: alarmtime;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 3;
        font.pointSize: 12;
		font.bold: true;
		text: triggerTime + "Uhr"
		elide: Text.ElideLeft
	}

	Switch{
		id: enaAlarm;
		anchors.top: parent.top;
		anchors.right: parent.right;
		anchors.margins: 5
		position: alarmEnabled;
		text: alarmEnabled ? qsTr("enabled") : qsTr("disabled")

		onToggled:{
			alarmlistmodel.set_enabled(index, position)
		}
	}
	    
}


