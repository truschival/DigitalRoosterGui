import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Item {
    property double temperature;
    property string timestamp;
    property url condition_icon;
    implicitHeight: 64;
    implicitWidth:  80;

    GridLayout{
        rows: 3;
        columns: 2;
        rowSpacing: 0;
        columnSpacing: -5;

        Text{
            id: temp_max;
            text: Math.round(weather.temperature) + "\u00B0C";
            font: Style.font.weatherInfo;
            color: "white"
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom;
        }
        
        Image {
            id: condition_icon;
            fillMode: Image.PreserveAspectFit

	    Layout.rowSpan: 2;
            Layout.margins: 0;
            Layout.leftMargin: 0;
            Layout.minimumHeight: 64;
            Layout.maximumHeight: 72;
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
        }

        Text{
            id: temp_min;
            text: Math.round(weather.temperature) + "\u00B0C";
            font: Style.font.weatherInfo;
            color: "white"
            Layout.alignment: Qt.AlignRight | Qt.AlignTop;
        }

        Text{
            id: timestamp;
            text: "16:00";
            font: Style.font.label;
            color: "white";
	    Layout.topMargin: -7;
            Layout.columnSpan: 2;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
        }
    }

    function update(weather){
	console.log("update: " + weather.timestamp);
        timestamp.text  = Qt.formatTime(weather.timestamp,"hh:mm");
        temp_max.text = Math.round(weather.temp_max)+"\u00B0C";
	temp_min.text = Math.round(weather.temp_min)+"\u00B0C";
	condition_icon.source = weather.icon_url;
    }

    MouseArea{
        anchors.fill: parent;
        pressAndHoldInterval: 500; //ms press to refresh
        onPressAndHold: {
            console.log("Refresh weather")
            weather.refresh();
        }
    }
}
