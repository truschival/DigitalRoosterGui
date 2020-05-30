import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

/**
 * Brightness Menu
 */
Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    dim: true;
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    Timer {
        id: brightnessMenuCloseTimer;
        interval: applicationWindow.dialogTimeout;
        running: false;
        repeat: false;
        onTriggered: parent.close();
    }

    onAboutToShow : {
        brightnessMenuCloseTimer.start();
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
