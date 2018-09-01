import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import "Jsutil.js" as Util

Rectangle{
    id: alarmDelegate
	width: parent.width;
    height: 60;
    radius: 3;
	border.width: 1;
	color: alarmEnabled ? "#3F51B5" :  "Grey" ;

	MouseArea {
		anchors.fill : parent;
		onPressAndHold: {
			console.log(index + " Pressed")
		}
	}

	
	RowLayout{
		anchors.fill: parent
		anchors.leftMargin: 10;
		anchors.rightMargin: 10;
		spacing: 10;
		anchors.verticalCenter: parent.verticalCenter;
		
		/*		
				ComboBox {
				id: period
				model: ListModel {
				id: model
				ListElement { text: qsTr("Once") }
				ListElement { text: qsTr("Daily") }
				ListElement { text: qsTr("Weekend") }
				ListElement { text: qsTr("Workdays") }
				}
				currentIndex: periodicity;
				}
		*/
		
		Text {
			id: periodicityString;
			text:  periodstring ;
       		Layout.fillWidth: true
			Layout.minimumWidth: 100
			Layout.preferredWidth: 200
			font.pointSize: 12;
		}

		Text {
			id: alarmtime;
			Layout.minimumWidth: 100
			Layout.preferredWidth: 200
			font.pointSize: 12;
			font.bold: true;
			text: Qt.formatTime(triggerTime, "hh:mm")
			elide: Text.ElideLeft
		}

		Switch{
			id: enaAlarm;
			Layout.minimumWidth: 100
			Layout.preferredWidth: 200

			position: alarmEnabled;
			text: alarmEnabled ? qsTr("enabled") : qsTr("disabled")

			onCheckedChanged:{
				alarmlistmodel.set_enabled(index, position)
			}
		}
	}
}


