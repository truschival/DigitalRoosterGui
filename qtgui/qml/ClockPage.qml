import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
    id: clockPage
    property string objectName : "ClockPage"

    GridLayout{
        columns: 3;
        rows: 3;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.slim;
	anchors.bottomMargin: Style.itemMargins.medium;
	rowSpacing: 0;

        Text{
            text: currentTime.timestring_lz_hh_mm
            font: Style.font.clock;
            color: "white"
            Layout.columnSpan: 3;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter ;
        }

        RowLayout{
            Layout.columnSpan: 3;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Text{
                text: Math.round(weather.temperature) + "\u00B0C";
                font: Style.font.subtitle;
                color: "white"
                Layout.alignment: Qt.AlignRight| Qt.AlignVCenter;
            }
            Image {
                source: weather.weatherIcon;
                fillMode: Image.PreserveAspectFit
                Layout.minimumHeight: 64;
                Layout.minimumWidth: 64;
                Layout.preferredWidth: 72;
                Layout.preferredHeight: 72;
                Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
                Layout.fillHeight: true;
            }
        }// Rowlayout Temp+Icon current

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
        console.log("update_forecast")
        var idx =0;
        for(idx =0 ; idx <5 ; idx++){
            console.log("Time"+ weather.get_forecast_timestamp(idx))
            console.log("\tTemp:" + weather.get_forecast_temperature(idx)+ "°C")
            console.log("\tURL:" +weather.get_forecast_icon_url(idx) );
        }
		fc0h.update(weather.get_forecast_timestamp(0),
                    weather.get_forecast_temperature(0),
                    weather.get_forecast_icon_url(0));

        fc6h.update(weather.get_forecast_timestamp(2),
                    weather.get_forecast_temperature(2),
                    weather.get_forecast_icon_url(2));

        fc12h.update(weather.get_forecast_timestamp(4),
                     weather.get_forecast_temperature(4),
                     weather.get_forecast_icon_url(4));

    }

    /* one signal->update all forecasts*/
    Component.onCompleted: {
        weather.forecast_available.connect(update_forecast)
    }
}//Page
