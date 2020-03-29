import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


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
            color: "white";
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
            color: "white"
            Layout.topMargin: -12;
            Layout.columnSpan: 2;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
        }
    }

    function update(weather){
        console.log("update: " + weather.timestamp);
        timestamp.text  = Qt.formatTime(weather.timestamp,"hh:mm");
        temp.text = Math.round(weather.temp)+"\u00B0C";
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
