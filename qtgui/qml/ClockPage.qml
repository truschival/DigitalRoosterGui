import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
    id: clockPage
    property string objectName : "ClockPage"

    GridLayout{
        columns: 2;
        rows: 3;
        anchors.fill: parent
        anchors.leftMargin:  Style.itemMargins.sparse;
        anchors.rightMargin: Style.itemMargins.sparse;
        anchors.bottomMargin:  Style.itemMargins.sparse;
        Text{
            text: currentTime.timestring_lz_hh_mm
            font: Style.font.clock;
            color: "white"
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter ;
            anchors.bottomMargin: Style.itemMargins.extrawide;
        }

        RowLayout{
            Layout.columnSpan: 2
            Layout.margins: Style.itemMargins.slim;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Text{
                text: Math.round(weather.temperature) + "\u00B0C";
                font: Style.font.subtitle;
                color: "white"
                Layout.alignment: Qt.AlignRight| Qt.AlignVCenter;
            }
            Image {
            	Layout.minimumHeight: 64;
            	Layout.minimumWidth: 64;
            	Layout.preferredWidth: 72;
                Layout.preferredHeight: 72;
                
                Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
                Layout.fillHeight: true;
                fillMode: Image.PreserveAspectFit
                source: weather.weatherIcon;
            }
        }// Rowlayout Temp+Icon current

        GridLayout{
            columns: 2;
            rows: 2;
            rowSpacing:0;
            columnSpacing: 0;// Style.itemMargins.slim;

            Layout.margins: Style.itemMargins.slim;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Text{
                id: temperature_6h;
                text: Math.round(weather.temperature) + "\u00B0C";
                font: Style.font.label;
                color: "white"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter;
            }
            Image {
                id: condition_icon_6h;
                fillMode: Image.PreserveAspectFit
                source: weather.weatherIcon;
            
                Layout.rowSpan: 2;         
                Layout.maximumWidth: 64
                Layout.maximumHeight: 64
                Layout.minimumWidth: 50
                Layout.minimumHeight: 50
                Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
                Layout.fillHeight: true;
            }
            Text{
                id: timestamp_6h;
                text: "6:00";
                font: Style.font.label;
                color: "white"
                
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop;
               // Layout.topMargin:   -Style.itemMargins.slim;
               // Layout.bottomMargin: Style.itemMargins.slim;
            }
        }// Gridlayout Temp+Icon +6h

        GridLayout{
                    columns: 2;
            rows: 2;
            rowSpacing:0;
            columnSpacing: Style.itemMargins.slim;

            Layout.margins:  Style.itemMargins.slim;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Text{
                id: temperature_12h;
                text: Math.round(weather.temperature) + "\u00B0C";
                font: Style.font.label;
                color: "white"
                Layout.alignment: Qt.AlignRight| Qt.AlignVCenter;
            }
            Image {
                id: condition_icon_12h;
                Layout.leftMargin:-2;
                Layout.maximumWidth: 64
                Layout.maximumHeight: 64
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
                Layout.fillHeight: true;
                fillMode: Image.PreserveAspectFit
                source: weather.weatherIcon;
            }
            Text{
                id: timestamp_12h;
                text: "12:00";
                font: Style.font.label;
                Layout.columnSpan: 2
                color: "white"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
                Layout.topMargin: -Style.itemMargins.slim;
                Layout.bottomMargin: Style.itemMargins.slim;
            }
        }// Gridlayout Temp+Icon +12h

    }// Gridlayout

    function update_forecast(){
        console.log("update_forecast")
        var idx =0;
        for(idx =0 ; idx <5 ; idx++){
            console.log("Time"+ weather.get_forecast_timestamp(idx))
            console.log("\tTemp:" + weather.get_forecast_temperature(idx)+ "°C")
            console.log("\tURL:" +weather.get_forecast_icon_url(idx) );
        }
        timestamp_6h.text = Qt.formatTime(weather.get_forecast_timestamp(2),"hh:mm");
        temperature_6h.text = Math.round(weather.get_forecast_temperature(2)) + "\u00B0C";
        condition_icon_6h.source = weather.get_forecast_icon_url(2);

        timestamp_12h.text = Qt.formatTime(weather.get_forecast_timestamp(4),"hh:mm");
        temperature_12h.text = Math.round(weather.get_forecast_temperature(4)) + "\u00B0C";
        condition_icon_12h.source = weather.get_forecast_icon_url(4);
    }

    /* one signal->update all forecasts*/
    Component.onCompleted: {
        weather.forecast_available.connect(update_forecast)
    }
}//Page
