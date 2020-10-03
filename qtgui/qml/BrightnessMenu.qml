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

	CheckBox {
            checked: brightnessControl.feedback
            text: "Adaptive mode"
            enabled: brightnessControl.has_sensor
      	    onClicked: {
      		brightnessControl.feedback = checked;
      	    }
      	}

        Text{
            text: "Active brightness:";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
        Slider {
            id: brightnessSlider
            orientation: Qt.Horizontal
            from: 5
            to: 99
            stepSize: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            value: brightnessControl.active_brightness
            onMoved: {
                brightnessControl.active_brightness = value;
                brightnessMenuCloseTimer.restart();
            }
        }

        Text{
            text: "Standby brightness:";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        Slider {
            id: standbySlider
            orientation: Qt.Horizontal
            from: 5
            to:  99
            stepSize: 1
            wheelEnabled: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            value: brightnessControl.standby_brightness
            onMoved: {
                brightnessControl.standby_brightness = value;
                brightnessMenuCloseTimer.restart();
            }
        }// Slider
    }// GridLayout
}// Brightness Menu
