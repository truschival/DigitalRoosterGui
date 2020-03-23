import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

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
        columnSpacing: 0;
	
        Text{
            text: currentTime.timestring_lz_hh_mm
            font: Style.font.clock;
            color: "white"
            Layout.columnSpan: 3;
    	    Layout.topMargin: Style.itemMargins.medium;
	    Layout.bottomMargin: Style.itemMargins.slim;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter ;
        }

        ForecastWidget{
            id:fc0h;
            timestamp: "+00 h";
	    Layout.margins: Style.itemMargins.slim;    
            Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter;
        }

        ForecastWidget{
            id:fc6h;
            timestamp: "+06 h";
	    Layout.margins: Style.itemMargins.slim;    
            Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter;
        }

        ForecastWidget{
            id:fc12h;
            timestamp: "+12 h";
	    Layout.margins: Style.itemMargins.slim;    
            Layout.alignment: Qt.AlignHCenter| Qt.AlignVCenter;
        }

    }// Gridlayout

    function update_forecast(){
        console.log("update_forecast")
        fc0h.update(weather.get_weather(0));
	fc6h.update(weather.get_weather(2));
	fc12h.update(weather.get_weather(4));
    }

    /* one signal->update all forecasts*/
    Component.onCompleted: {
        weather.forecast_available.connect(update_forecast)
    }
}//Page
