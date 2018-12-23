import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

import ruschi.PlayableItem 1.0

Rectangle{
	id: iradiodelegatearea
	width: parent.width;
    height: Style.contentHeight/5;
    radius: 3;
	border.width: 1;
	color: iradiolist.currentItem == this ?
		Style.colors.selected : Style.colors.unselected ;

	RowLayout{
		anchors.fill: parent
		anchors.margins: Style.itemMargins.medium;
		spacing: Style.itemSpacings.sparse
		anchors.verticalCenter: parent.verticalCenter;

		Text {
			id:stationName
			text: station_name;
			Layout.preferredWidth: iradiodelegatearea.width
			elide: Text.ElideRight
			font: Style.font.listItemHead;
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked:{
			playerControlWidget.show()
		}
		onPressAndHold:  {
			iradiolist.currentIndex =index;
			iradiolistmodel.send_to_player(index)
			playerControlWidget.setCurrentMediaTitle(station_name)
			playerControlWidget.show()
		}
	}
}
