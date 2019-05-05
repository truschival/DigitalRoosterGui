import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
    id: clockPage
    property string objectName : "ClockPage"

    ColumnLayout{
        anchors.fill: parent
        anchors.leftMargin: Style.itemMargins.extrawide;
        anchors.rightMargin:Style.itemMargins.extrawide;
        spacing: Style.itemSpacings.sparse;

        Text{
            text: currentTime.timestring_lz_hh_mm
            font: Style.font.clock;
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        /* Text{ */
        /* 	text: currentTime.datestring_lz */
        /* 	font: Style.font.subtitle; */
        /* 	color: "white" */
        /* 	Layout.alignment: Qt.AlignHCenter */
        /* } */

        GridLayout{
            id: weatherLayout
            columns: 2
            rows: 2
            rowSpacing: 0;
            columnSpacing: Style.itemSpacings.dense;

            Layout.alignment: Qt.AlignHCenter
            Text{
                text: weather.city + " " +  Math.round(weather.temperature*10)/10 + "\u00B0C";
                font: Style.font.subtitle;
                color: "white"
                Layout.columnSpan: 1
                Layout.alignment: Qt.AlignHCenter
            }

            Image {
                id: cloudicon
                Layout.maximumWidth: 72
                Layout.maximumHeight: 72
                Layout.preferredWidth: 64
                Layout.preferredHeight: 64
                Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter
                Layout.fillHeight: true;

                fillMode: Image.PreserveAspectFit
                source: weather.weatherIcon;
            }
        }

    }
}
