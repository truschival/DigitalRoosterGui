import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Item {
    property double temperature;
    property string timestamp;
    property url condition_icon;
    implicitHeight: 50;
    implicitWidth: 85;

    GridLayout{
        columns: 2;
        rows: 2;
        rowSpacing:0;
        columnSpacing: 0;

        Text{
            id: temperature;
            text: Math.round(weather.temperature) + "\u00B0C";
            font: Style.font.label;
            color: "white"
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter;
        }

	Image {
            id: condition_icon;
            fillMode: Image.PreserveAspectFit
            Layout.rowSpan: 2;
	    Layout.margins: 0;
            Layout.maximumWidth: 72
            Layout.maximumHeight: 72
            Layout.minimumWidth: 64
            Layout.minimumHeight: 64
            Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
            Layout.fillHeight: true;
        }

	Text{
            id: timestamp;
            text: "6:00";
            font: Style.font.label;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop;
        }
    }

    function update(ts, temp, iconurl){
        console.log("update: "+temp+ " : "+ts+" : "+iconurl)
        timestamp.text  = Qt.formatTime(ts,"hh:mm");
        temperature.text = Math.round(temp)+"\u00B0C";
        condition_icon.source = iconurl;
    }
}
