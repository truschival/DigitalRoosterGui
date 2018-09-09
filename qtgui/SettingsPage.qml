import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
	id: settingsPage
	property string objectName : "SettingsPage"

	GridLayout{
		columns: 2
		rows: 3
		rowSpacing: 2
		columnSpacing:2
		anchors.fill: parent
		anchors.leftMargin: 10;
		anchors.rightMargin: 10;
		Layout.alignment: Qt.AlignHCenter

		Text{
			text: "Revision: "+ config.revision;
			font.pointSize: 12
			color: "white"
			Layout.maximumHeight:25
			Layout.preferredHeight:20
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}

		Text{
			text: "Build: " + config.buildtime;
			font.pointSize: 12
			color: "white"
			Layout.maximumHeight:25
			Layout.preferredHeight:20
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}

		Text{
			text: "Brightness (ON):";
			font.pointSize: 12
			color: "white"
			Layout.maximumHeight:25
			Layout.preferredHeight:20
			Layout.columnSpan: 1
			Layout.topMargin: 8
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
		}

		Slider {
			id: brightnessSlider
            orientation: Qt.Horizontal
            from: 5
            to: 100
            stepSize: 5
			wheelEnabled: true
			// change that to class for handling brightness control
            value: brightnessControl.brightness
            onMoved: {
               	brightnessControl.brightness = value;
            }
			Layout.topMargin: 8
			Layout.minimumWidth : 250
			Layout.preferredWidth: 300
			Layout.maximumWidth: 400
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
		}

		Text{
			text: "Brightness (OFF):";
			font.pointSize: 12
			color: "white"
			Layout.maximumHeight:25
			Layout.preferredHeight:20
			Layout.columnSpan: 1
			Layout.topMargin: 8
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
		}

		Slider {
			id: standbySlider
            orientation: Qt.Horizontal
            from: 5
            to: 100
            stepSize: 5
			wheelEnabled: true
			// change that to class for handling brightness control
            value: config.standbybrightness
            onMoved: {
               	config.standbybrightness = value;
            }
			Layout.topMargin: 8
			Layout.minimumWidth : 250
			Layout.preferredWidth: 300
			Layout.maximumWidth: 400
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
		}


		Rectangle{
			id: fillerRect
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignLeft
			Layout.fillHeight: true
			radius: 3;
			border.width: 1;
			color: "Grey" ;
		}

	}

}
