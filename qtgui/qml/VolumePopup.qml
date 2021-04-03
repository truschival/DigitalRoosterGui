// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.4

Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;
    Timer {
        id: volumeShowTimer
        interval: 3000
        running: true
        repeat: false
        onTriggered: volumePopUp.close()
    }

    contentItem: GridLayout {
        anchors.fill: parent;
        rowSpacing: Style.itemSpacings.dense;
        columnSpacing: Style.itemSpacings.dense;
        rows: 2;
        columns:3;

        // Row 1
        Label{
            font: Style.font.sliderValue
            text: volumeSlider.value
            Layout.alignment: Qt.AlignVCenter| Qt.AlignHCenter
            Layout.columnSpan: 3;
        }
        // Row 2
        IconLabel{
            // volume low
            text: "\uf57f";
            color: "white";
        }

        Slider {
            id: volumeSlider
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true;
            from: 0
            to: 100
            stepSize: 1
            wheelEnabled: true
            value: playerProxy.volume
            onMoved: {
                volumeShowTimer.restart();
                playerProxy.volume = value;
            }
        }
        IconLabel{
            //volume high
            text: "\uf57e";
            color: "white";
        }

    }
    function show(){
        volumeShowTimer.start();
        volumePopUp.open();
    }
}
