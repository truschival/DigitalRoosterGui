import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

import ruschi.PlayableItem 1.0

Rectangle{
	id: iradiodelegatearea
	width:  parent.width;
    height: 60;
    radius: 3;
	border.width: 1;
	color: iradiolist.currentItem == this ? "lightblue" : "white"

	RowLayout{
		anchors.fill: parent
		anchors.leftMargin: 10;
		anchors.rightMargin: 10;
		spacing: 10;
		anchors.verticalCenter: parent.verticalCenter;

		Text {
			id:stationName
			text: station_name;
			Layout.preferredWidth: iradiodelegatearea.width

			font.pointSize: 14;
			font.bold: true;
			elide: Text.ElideRight
		}
	}


	MouseArea {
		anchors.fill: parent
		onClicked:{
			playerControlWidget.setVisible(true)
		}
		onPressAndHold:  {
			iradiolist.currentIndex =index;
			iradiolistmodel.send_to_player(index)
			playerControlWidget.setCurrentMediaTitle(station_name)
			playerControlWidget.setVisible(true)
		}
	}
}
