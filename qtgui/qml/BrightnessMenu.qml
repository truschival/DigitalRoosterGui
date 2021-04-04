// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

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
                brightnessMenuCloseTimer.restart();
            }
        }

        Text{
            text: "Active brightness:";
            font: Style.font.boldLabel;
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.leftMargin: Style.itemMargins.wide;
        }
        Slider {
            id: brightnessSlider
            orientation: Qt.Horizontal
            from: 10;
            to: 99;
            stepSize: 1;
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
            color: Style.colors.primaryText;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.leftMargin: Style.itemMargins.wide;
        }

        Slider {
            id: standbySlider
            orientation: Qt.Horizontal
            from: 2;
            to:  70;
            stepSize: 1;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            value: brightnessControl.standby_brightness
            onMoved: {
                brightnessControl.standby_brightness = value;
                brightnessMenuCloseTimer.restart();
            }
        }// Slider
    }// GridLayout
}// Brightness Menu
