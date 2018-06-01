import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Rectangle{
    id: rectangle
	width:  parent.width;
    height: 50;
    radius: 3;
	border.width: 1;
	border.color: "#9E9E9E" ;

	Text {
        id:stationName
        width: parent.width;
		height: parent.height;
        text: periodicity;
		anchors.top: parent.top;
		anchors.topMargin: 3;
        font.pointSize: 14;
		font.bold: true;
		elide: Text.ElideRight
	}
	    
}


