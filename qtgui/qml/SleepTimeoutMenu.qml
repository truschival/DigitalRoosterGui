import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4

import "Jsutil.js" as Util

Menu {
	focus: true
	closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

	enter: Transition {
		NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 ; duration: 300}
	}
	exit: Transition {
		NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 ; duration: 400}
	}

    contentItem: GridLayout {
		anchors.fill: parent;
		anchors.margins: Style.itemMargins.slim;
        rows: 3;
		columns:2;
        columnSpacing: Style.itemSpacings.medium;
        rowSpacing: Style.itemSpacings.slim;
    
        
        Text{
            text: "Sleep timeout in minutes" ;
            horizontalAlignment: Text.AlignHCenter
            font: Style.font.label;
            color: "white";
            elide: Text.ElideRight;

            Layout.topMargin: Style.itemMargins.wide;
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        }

		Tumbler{
			id: sleepTimeoutTumbler
            Layout.maximumHeight: 100;
            Layout.preferredHeight: 90;
            Layout.fillHeight: true;
            Layout.rowSpan: 2;
            Layout.margins: Style.itemMargins.medium;
			Layout.alignment: Qt.AlignCenter| Qt.AlignTop
			
			TumblerColumn {
				id: sleepTimeoutMinutes
				model: [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120];
				width:  64;
				delegate: Text {
                    text: styleData.value;
					font: Style.font.tumbler;
					horizontalAlignment: Text.AlignHCenter
                    opacity: 0.3 + Math.max(0, 1 - Math.abs(styleData.displacement)) * 0.7
				}
			}
		}
        Button{
            id: sleepTimeoutMenuOK;
            text: "Ok";
            Layout.preferredWidth: sleepTimeoutMenuCancel.width;
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            onClicked: {
                var minutes = (sleepTimeoutTumbler.currentIndexAt(0)+1) *10;
                console.log(" new sleep timeout: " + minutes + " IDX:" + sleepTimeoutTumbler.currentIndexAt(0));
                sleeptimer.sleep_timeout_min = minutes;
            }
        }

        Button{
            id: sleepTimeoutMenuCancel;
            text: "Cancel"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            onClicked: {
                sleepTimeoutMenu.close();
            }
        }
		
	} // Gridlayout

	onAboutToShow : {
		var idx = (sleeptimer.sleep_timeout_min/10)-1
		console.log(" new sleep timeout: " + sleeptimer.sleep_timeout_min+ " IDX:" + idx);
		sleepTimeoutTumbler.setCurrentIndexAt(0, idx);
	}

	onAboutToHide : {
        // nothing
	}
}
