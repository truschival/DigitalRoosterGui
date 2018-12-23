import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
	id: clockPage
	property string objectName : "ClockPage"

	ColumnLayout{
		anchors.fill: parent
		anchors.leftMargin: Style.itemMargins.extrawide;
		anchors.rightMargin:Style.itemMargins.extrawide;
		spacing: Style.itemSpacings.sparse;

		Text{
			text: currentTime.timestring_lz_hh_mm
			font: Style.font.clock;
			color: "white"
			Layout.alignment: Qt.AlignHCenter
		}

		Text{
			text: currentTime.datestring_lz
			font: Style.font.title;
			color: "white"
			Layout.alignment: Qt.AlignHCenter
		}

		GridLayout{
			id: weatherLayout
			columns: 2
			rows: 2
			rowSpacing: 0;
			columnSpacing: Style.itemSpacings.dense;
			
			Layout.alignment: Qt.AlignHCenter
			Text{
				text: weather.city + " " +  Math.round(weather.temperature*10)/10 + "\u00B0C";
				font: Style.font.subtitle;
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
				font: Style.font.subtitle;
				horizontalAlignment: Text.AlignHCenter
				anchors.topMargin:-2;
				Layout.fillWidth: true;
				Layout.columnSpan: 2;
				Layout.alignment: Qt.AlignHCenter| Qt.AlignTop
			}
		}

	}
}
