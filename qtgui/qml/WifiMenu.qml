import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

/**
 * Wifi Menu
 */
Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    dim: true;
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    GridLayout{
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
            text: "Press for WPS PBC";
            font: Style.font.label;
            color: "white";
            Layout.rightMargin: Style.itemMargins.wide;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
        }

        WifiList{
            id: wifilistview;
            Layout.fillWidth: true;
            Layout.margins: Style.itemMargins.wide;
            Layout.fillHeight: true;
            Layout.columnSpan: 2;
        }
    }// ColumnLayout
}// Wifi Menu
