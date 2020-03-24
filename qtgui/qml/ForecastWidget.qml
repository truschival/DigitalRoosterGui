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
        rows: 2;
        columns: 2;
        rowSpacing: 0;
        columnSpacing:0;

        Text{
            id: timestamp;
            text: "16:00";
            font: Style.font.label;
            color: "white";
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
        }

        Image {
            id: condition_icon;
            fillMode: Image.PreserveAspectFit
	    Layout.margins: -10;
            Layout.minimumHeight: 64;
            Layout.maximumHeight: 72;
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter;
        }

        Text{
            id: temps;
            text: "-12 | 12 \u00B0C";
            font: Style.font.weatherInfo;
            color: "white"
	    Layout.topMargin: -5;
	    Layout.columnSpan:2;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
        }

    }

    function update(weather){
	console.log("update: " + weather.timestamp);
        timestamp.text  = Qt.formatTime(weather.timestamp,"hh:mm");
        temps.text = Math.round(weather.temp_min)+" | "+
	    Math.round(weather.temp_max)+" \u00B0C";

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
