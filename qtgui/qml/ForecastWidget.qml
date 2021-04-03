// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Rectangle {
    property double temperature;
    property string timestamp;
    property url condition_icon;

    height: 80;
    width:  100;
    color: "transparent";
    // border.width: 1;
    // border.color: "green";

    GridLayout{
        rows: 2;
        columns: 2;
        rowSpacing: 0;
        columnSpacing:0;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.slim;
        anchors.topMargin: 0;

        Text{
            id: temp;
            text: "-12\u00B0C"
            font: Style.font.weatherInfo;
            color: Style.colors.primaryText;
            style: Text.Outline;
            styleColor: Style.colors.selected;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter;
            z:1; //cover icon if needed
        }

        Image {
            id: condition_icon;
            fillMode: Image.PreserveAspectFit
            Layout.margins: -8;
            Layout.minimumWidth: 50;
            Layout.preferredWidth: 60;
            Layout.maximumWidth: 70;
            Layout.fillHeight: true;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter;
        }

        Text{
            id: timestamp;
            text: "23:55";
            font: Style.font.weatherTime;
            color: Style.colors.primaryText;
            Layout.topMargin: -12;
            Layout.columnSpan: 2;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
        }
    }

    function update(weather){
        timestamp.text  = Qt.formatTime(weather.timestamp,"hh:mm");
        temp.text = Math.round(weather.temp)+"\u00B0C";
        condition_icon.source = weather.icon_url;
    }

    MouseArea{
        anchors.fill: parent;
        pressAndHoldInterval: 500; //ms press to refresh
        onPressAndHold: {
            weather.refresh();
        }
    }
}
