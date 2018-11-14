import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import ruschi.Alarm 1.0

Popup {
	property Alarm currentAlarm;
	focus: true

	closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

	enter: Transition {
		NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 ; duration: 300}
	}
	exit: Transition {
		NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 ; duration: 400}
	}

    contentItem: GridLayout {
		rowSpacing: 2;
		columnSpacing: 6;
		rows: 2;
		columns:2;

		Tumbler{
			id: timeTumbler
			//Layout.maximumWidth: 80
			Layout.maximumHeight: 96 
			Layout.rowSpan: 2
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			Layout.fillHeight: true;
			
			TumblerColumn {
				id: hoursTumbler
				model: 24
			}
			TumblerColumn {
				id: minutesTumbler
				model: 60
			}
		}
		//----------
		
		Switch{
			id: enaAlarm;
			Layout.minimumWidth: 100
			Layout.preferredWidth: 200
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			position: alarmlistmodel.get_alarm(alarmlistmodel.currentIndex).enabled;
			text: currentAlarm.enabled ? qsTr("enabled") : qsTr("disabled")
			
			onCheckedChanged:{
				alarmlistmodel.set_enabled(alarmlistmodel.currentIndex, position)
			}
		}
		
		ComboBox {
			id: period
			model: ListModel {
				id: model
				ListElement { text: qsTr("Once") }
				ListElement { text: qsTr("Daily") }
				ListElement { text: qsTr("Weekend") }
				ListElement { text: qsTr("Workdays") }
			}
			currentIndex: currentAlarm.period_id;
			
			onActivated: {
				console.log("new index" + currentIndex);
				currentAlarm.period_id = currentIndex;
			}
		}
		

	}
}
