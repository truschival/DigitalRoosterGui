import QtQuick 2.9
import ruschi.PlayableItem 1.0

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
        text: station_name;
		anchors.top: parent.top;
		anchors.topMargin: 3;
        font.pointSize: 14;
		font.bold: true;
		elide: Text.ElideRight
	}
	    
	MouseArea {
		anchors.fill: parent
		onClicked:{
			playerControlWidget.setVisible(true)
		}
		onPressAndHold:  {
			iradiolistmodel.send_to_player(index)
			playerControlWidget.setVisible(true)
		}
	}

}


