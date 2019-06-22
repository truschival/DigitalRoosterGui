import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

/**
 * Power (-Off) Menu
 */
Menu {
    enter: Transition {
        NumberAnimation { property: "opacity";
            from: 0.0; to: 1.0 ; duration: 300}
    }
    exit: Transition {
        NumberAnimation { property: "opacity";
            from: 1.0; to: 0.0 ; duration: 400}
    }

    GridLayout {
        rows: 2;
        columns: 2;
        rowSpacing: Style.itemSpacings.medium;
        columnSpacing: Style.itemSpacings.medium;

        IconButton {
            id: poweroffBtn
            text: "\uf901"
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                console.log("power off button")
                powerControl.power_off();
            }
        }
        Text{
            text: "shutdown";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }


        IconButton {
            id: rebootBtn
            text: "\uf900";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                console.log("reboot button")
                powerControl.reboot();
            }
        }
        Text{
            text: "reboot";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
    }
}
