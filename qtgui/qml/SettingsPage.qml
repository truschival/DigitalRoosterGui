import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
	id: settingsPage
	property string objectName : "SettingsPage"
	
	GridLayout{
		anchors.fill: parent
		columns: 2
		rows: 3
		rowSpacing: Style.itemSpacings.dense;
		columnSpacing:  Style.itemSpacings.dense;
		anchors.margins: Style.itemMargins.medium;

		Text{
			text: "Revision: ";
			font: Style.font.label;
			color: "white"
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}
		Text{
			text: config.revision;
			font: Style.font.valueLabel;
			color: "white"
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}

		Text{
			text: "Build: ";
			font: Style.font.label;
			color: "white"
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}
		
		Text{
			text: config.buildtime;
			font: Style.font.valueLabel;
			color: "white"
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}

		GroupBox{
			Layout.columnSpan: 2

			GridLayout{
				anchors.fill: parent;
				columns: 2
				rows: 2
				rowSpacing: 0
				anchors.margins: Style.itemMargins.slim;
				columnSpacing:  Style.itemSpacings.dense;		
				
				Text{
					text: "Brightness";
					font: Style.font.label;
					color: "white"
					Layout.columnSpan: 2
				}

				Text{
					text: "Active:";
					font: Style.font.label;
					color: "white"
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
				}

				Text{
					text: "Standby:";
					font: Style.font.label;
					color: "white"
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
				}// Slider
			}// GridLayout
		}// GroupBox	
	}// GridLayout
}
