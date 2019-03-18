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

    ColumnLayout{
        width: wifiMenu.width;
        height: wifiMenu.height;
        anchors.leftMargin: Style.itemMargins.wide;
        anchors.rightMargin: Style.itemMargins.wide;
        spacing: 1;
		clip: true;
		
        Text{
            text: "Press network for WPS PBC";
            font: Style.font.label;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        WifiList{
            id: wifilistview;
            Layout.fillWidth: true;
            Layout.margins: Style.itemMargins.wide;
            Layout.minimumHeight: parent.height*0.8;
            
        }
    }// ColumnLayout
}// Wifi Menu
