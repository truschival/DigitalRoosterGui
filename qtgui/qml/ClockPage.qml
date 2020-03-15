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
	        text: Math.round(weather.temperature*10)/10 + "\u00B0C";
	        font: Style.font.subtitle;
	        color: "white"
	        Layout.alignment: Qt.AlignRight| Qt.AlignVCenter;
	    }
	    Image {
	        Layout.maximumWidth: 72
	        Layout.maximumHeight: 72
	        Layout.preferredWidth: 72
	        Layout.preferredHeight: 72
	        Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter;
	        Layout.fillHeight: true;
	        fillMode: Image.PreserveAspectFit
	        source: weather.weatherIcon;
	    }
        }// Rowlayout Temp+Icon current

      	/* Text{ */
        /*     text: weather.city; */
        /*     font: Style.font.label; */
        /*     color: "white" */
        /*     Layout.columnSpan: 2 */
        /*     Layout.margins: 0; */
        /*     Layout.alignment: Qt.AlignHCenter */
        /* } */

        GridLayout{
       	    columns: 2;
    	    rows: 2;
    	    rowSpacing:0;
	    columnSpacing: Style.itemMargins.slim;

	    Layout.margins: Style.itemMargins.slim;
	    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

	    Text{
	        text: Math.round(weather.temperature) + "\u00B0C";
	        font: Style.font.label;
	        color: "white"
	        Layout.alignment: Qt.AlignRight| Qt.AlignVCenter;
	    }
	    Image {
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
	        text: "12:00";
	        font: Style.font.label;
	        Layout.columnSpan: 2
	        color: "white"
	        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
	        Layout.topMargin: -Style.itemMargins.slim;
	        Layout.bottomMargin: Style.itemMargins.slim;
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
	        text: Math.round(weather.temperature) + "\u00B0C";
	        font: Style.font.label;
	        color: "white"
	        Layout.alignment: Qt.AlignRight| Qt.AlignVCenter;
	    }
	    Image {
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
	        text: "18:00";
	        font: Style.font.label;
	        Layout.columnSpan: 2
	        color: "white"
	        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
	        Layout.topMargin: -Style.itemMargins.slim;
	        Layout.bottomMargin: Style.itemMargins.slim;
	    }
        }// Gridlayout Temp+Icon +12h

    }// Gridlayout
}//Page
