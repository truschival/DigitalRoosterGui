import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import ruschi.PodcastEpisode 1.0

Rectangle{
	id: rectangle
	width:  parent.width;
	height: 100;
	color: episodemodel.currentIndex == index ? "lightblue" : "white"
	anchors.margins: 3

	Text {
		id:episodeDN
		width: parent.width;
		text: display_name;
		height: 28;
		anchors.top: parent.top;
		anchors.topMargin: 2;
		font.pointSize: 14;
		font.bold: true;
		elide: Text.ElideRight
	}

	Text {
		id:episodePubDate
		text: qsTr("Published")+": " + pub_date ;
		anchors.leftMargin: 2
		width: parent.width/2-4;
		height: 28;
		anchors.top: episodeDN.bottom
		anchors.topMargin: 2
		anchors.left: parent.left;
		font.pointSize: 12;
		elide: Text.ElideRight
	}

	Text {
		id:episodeDuration
		text: qsTr("Duration")+": "+ duration ;
		anchors.rightMargin: 2
		width: parent.width/2-4;
		height: 28;
		anchors.top: episodeDN.bottom
		anchors.topMargin: 2
		anchors.right: parent.right
		font.pointSize: 12;
		elide: Text.ElideRight
	}

	Text {
		id:episodeDescription
		text: description ;
		wrapMode: Text.WordWrap
		font.pointSize: 10

		width: parent.width-4;
		anchors.top: episodePubDate.bottom
		anchors.topMargin: 2
		anchors.left:parent.left
		anchors.leftMargin: 2
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 2
		elide: Text.ElideRight
	}

	MouseArea {
		anchors.fill: parent
		onClicked:{
			playerControlWidget.setVisible(true)
		}
		onPressAndHold:  {
			episodemodel.currentIndex =index;
			episodemodel.send_to_player(index)
			playerControlWidget.setCurrentMediaTitle(display_name)
			playerControlWidget.setVisible(true)
		}
	}

}


