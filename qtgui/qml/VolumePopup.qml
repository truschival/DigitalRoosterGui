import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Popup {
	focus: true
	closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

	enter: Transition {
		NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 ; duration: 300}
	}
	exit: Transition {
		NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 ; duration: 400}
	}

	Timer {
		id: volumeShowTimer
		interval: 3000
		running: true
		repeat: false
		onTriggered: volumePopUp.close()
	}

    contentItem: GridLayout {
		anchors.fill: parent;
		rowSpacing: Style.itemSpacings.dense;
		columnSpacing: Style.itemSpacings.dense;
		rows: 2;
		columns:3;

		// Row 1
		Label{
			font: Style.sliderValueFont
			text: volumeSlider.value
			Layout.alignment: Qt.AlignVCenter| Qt.AlignHCenter
			Layout.columnSpan: 3
		}
		// Row 2
		IconLabel{
			// volume low
			text: "\uf57f";
		}

		Slider {
			id: volumeSlider
			Layout.alignment: Qt.AlignVCenter
			Layout.fillWidth: true;
			from: 0
			to: 100
			stepSize: 1
			wheelEnabled: true
			value: playerProxy.volume
			onMoved: {
				volumeShowTimer.restart();
				playerProxy.volume = value;
			}
		}
		IconLabel{
			//volume high
			text: "\uf57e";
		}

	}
	function show(){
		volumeShowTimer.start();
		volumePopUp.open();
	}
}
