import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

/**
 * Wifi Menu
 */
Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    enter: Transition {
        NumberAnimation { property: "opacity";
            from: 0.0; to: 1.0 ; duration: 300}
    }
    exit: Transition {
        NumberAnimation { property: "opacity";
            from: 1.0; to: 0.0 ; duration: 400}
    }

    GridLayout{
        width: wifiMenu.width;
        height: wifiMenu.height;
        anchors.leftMargin: Style.itemMargins.slim;
        anchors.rightMargin: Style.itemMargins.slim;
        anchors.topMargin: Style.itemMargins.slim;
		clip: true;
		rows:3;
		columns:2;

		IconButton{
		  text: "\uf453";
		  Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
		  Layout.leftMargin: Style.itemMargins.wide;
		  onPressed : {
		      console.log("scan");
		      wifictrl.start_scan();
		  }
		}

        Text{
            id: wifictrlStatus;
            text: "Press for WPS PBC authentication";
            font: Style.font.label;
            color: "white";
            Layout.rightMargin: Style.itemMargins.wide;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
        }

        WifiList{
            id: wifilistview;
            Layout.fillWidth: true;
            Layout.margins: Style.itemMargins.wide;
            Layout.minimumHeight: parent.height*0.8;
            Layout.columnSpan: 2;
        }
    }// ColumnLayout
}// Wifi Menu
