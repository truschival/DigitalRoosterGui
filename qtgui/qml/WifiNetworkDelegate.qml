// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.4

Rectangle{
    width: parent.width;
    height: Style.contentHeight/4;
    radius: 3;
    border.width: 1;
    color: connected ? 	Style.colors.enabled : Style.colors.disabled;

    MouseArea {
        anchors.fill: parent
        onPressAndHold:  {
            wifilistmodel.currentIndex =index;
            console.log("Wifi pressed : "+index);
            wifilistmodel.wps_connect(index);
        }
    }
    
    RowLayout{
        anchors.fill: parent
        anchors.margins: Style.itemMargins.slim;
        spacing: Style.itemSpacings.dense;

        IconLabel {
            text: icon_for_signal(signal);
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
        }

        Text {
            id: network_name;
            text: name;
            font: Style.font.label;
            elide: Text.ElideRight;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
            Layout.fillWidth: true;
        }
    }

    function icon_for_signal(sig){
        if (sig < -90){
            return "\uf92e"
        } else if  (sig < -80){
            return "\uf91e";
        }  else if  (sig < -70){
            return "\uf921";
        } else if  (sig < -60){
            return "\uf924";
        }
        return "\uf927"; // full
    }
}
