import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
	id: clockPage
	property string objectName : "ClockPage"

	ColumnLayout{
		anchors.fill: parent
		anchors.leftMargin: 10;
		anchors.rightMargin: 10;
		spacing: 5;

		Text{
			text: currentTime.timestring_lz
			font.pointSize: 32
			color: "white"
			Layout.alignment: Qt.AlignHCenter
		}

		Text{
			text: currentTime.datestring_lz
			font.pointSize: 22
			color: "white"
			Layout.alignment: Qt.AlignHCenter
		}

		GridLayout{
			id: weatherLayout
			columns: 2
			rows: 2
			rowSpacing: 0
			columnSpacing:2
			Layout.alignment: Qt.AlignHCenter
			Text{
				text: weather.city + " " +  Math.round(weather.temperature*10)/10 + "\u00B0C";
				font.pointSize: 18
				color: "white"
				Layout.columnSpan: 1
				Layout.alignment: Qt.AlignHCenter
			}

			Image {
				id: cloudicon
				Layout.maximumWidth: 64
				Layout.maximumHeight: 64
				Layout.preferredWidth: 48
				Layout.preferredHeight: 48
				Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter
				Layout.fillHeight: true;

				fillMode: Image.PreserveAspectFit
				source: weather.weatherIcon;
			}
			Text{
				text: weather.condition
				color: "white"
				font.pointSize: 16
				horizontalAlignment: Text.AlignHCenter
				Layout.fillWidth: true
				Layout.columnSpan: 2
				Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter
			}
		}

	}
}
