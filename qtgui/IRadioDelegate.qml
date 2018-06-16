import QtQuick 2.9
import ruschi.PlayableItem 1.0

Rectangle{
	id: rectangle
	width:  parent.width;
	height: 50;
	anchors.margins: 3
	color: iradiolist.currentItem == this ? "lightblue" : "white"

	Text {
		id:stationName
		width: parent.width*0.9;
		height: parent.height;
		text: station_name;
		anchors.verticalCenter: parent.verticalCenter;
		anchors.left: parent.left;
		anchors.leftMargin: 10;
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
			iradiolist.currentIndex =index;
			iradiolistmodel.send_to_player(index)
			playerControlWidget.setVisible(true)
		}
	}

}


