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

        IconButton {
            id: brightnessBtn;
            text: "\uf0de";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                console.log("change brightness")
                brightnessMenu.popup((applicationWindow.width- brightnessMenu.width)/2,
                                     (applicationWindow.height- brightnessMenu.height)/2 - Style.itemMargins.extrawide)
            }
        }
        Text{
            text: "change brightness";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        IconButton {
            id: wifiBtn;
            text: "\uf5a9";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            onClicked: {
                console.log("change wifi")
            }
        }
        Text{
            text: "Wifi";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

     /**
     * Brightness Menu
     */
        Menu {
            id: brightnessMenu
            title: "Brightness";
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
                anchors.leftMargin: Style.itemMargins.wide;
                anchors.rightMargin: Style.itemMargins.wide;
                spacing: 0;

                Text{
                    text: "Active brightness:";
                    font: Style.font.boldLabel;
                    color: "white"
                    Layout.leftMargin: Style.itemMargins.wide;
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }
                Slider {
                    id: brightnessSlider
                    orientation: Qt.Horizontal
                    from: 1
                    to: 100
                    stepSize: 2
                    wheelEnabled: true
                    // change that to class for handling brightness control
                    value: brightnessControl.brightness
                    onMoved: {
                        brightnessControl.brightness = value;
                    }
                }

                Text{
                    text: "Standby brightness:";
                    font: Style.font.boldLabel;
                    color: "white"
                    Layout.leftMargin: Style.itemMargins.wide;
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }

                Slider {
                    id: standbySlider
                    orientation: Qt.Horizontal
                    from: 1
                    to: 100
                    stepSize: 2
                    wheelEnabled: true
                    // change that to class for handling brightness control
                    value: config.standbybrightness
                    onMoved: {
                        config.standbybrightness = value;
                    }
                }// Slider
            }// GridLayout
        }// Brightness Menu
    }// GridLayout
}
