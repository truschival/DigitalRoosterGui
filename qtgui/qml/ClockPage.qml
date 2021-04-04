// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Page {
    id: clockPage
    property string objectName : "ClockPage"

    GridLayout{
        columns: 3;
        rows: 2;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.slim;
        anchors.bottomMargin: Style.itemMargins.slim;
        rowSpacing: 0;
        columnSpacing: 3;

        Text{
            text: currentTime.timestring_lz_hh_mm
            font: Style.font.clock;
            color: Style.colors.primaryText
            Layout.columnSpan: 3;
            Layout.topMargin: Style.itemMargins.medium;
            Layout.bottomMargin:0;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter ;
        }

        ForecastWidget{
            id:fc0h;
            timestamp: "+00 h";
            Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter;
        }

        ForecastWidget{
            id:fc6h;
            timestamp: "+06 h";
            Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter;
        }

        ForecastWidget{
            id:fc12h;
            timestamp: "+12 h";
            Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter;
        }

    }// Gridlayout

    function update_forecast(){
        fc0h.update(weather.get_weather(0));
        fc6h.update(weather.get_weather(2));
        fc12h.update(weather.get_weather(4));
    }

    /* one signal->update all forecasts*/
    Component.onCompleted: {
        weather.forecast_available.connect(update_forecast)
    }
}//Page
