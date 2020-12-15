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

		Text{
            text: "IP: ";
            font: Style.font.label;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        Text{
            text: netinfo.ipaddr;
            font: Style.font.valueLabel;
            color: netinfo.linkstatus? "white" : Style.colors.disabled;
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        }

        IconButton {
            id: brightnessBtn;
            text: "\uf0de";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                brightnessMenu.popup((applicationWindow.width- brightnessMenu.width)/2,
                                     (applicationWindow.height- brightnessMenu.height)/2)
            }
        }
        Text{
            text: "change brightness";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        IconButton {
            id: sleeptimeoutBtn;
            text: "\uf51b";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                sleepTimeoutMenu.popup((applicationWindow.width- sleepTimeoutMenu.width)/2,
                                       (applicationWindow.height- sleepTimeoutMenu.height)/2);
            }
        }
        Text{
            text: "sleep timeout";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        IconButton {
            id: wifiBtn;
            text: "\uf5a9";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                wifiMenu.popup((applicationWindow.width- wifiMenu.width)/2,
                               (applicationWindow.height- wifiMenu.height)/2);
            }
        }
        Text{
            text: "Wifi";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
    }// GridLayout
}
