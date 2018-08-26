import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import ruschi.PodcastEpisode 1.0

Rectangle{
	id: rectangle
	width:  stackView.width-2;
	height: 100;
	color: episodemodel.currentIndex == index ? "lightblue" : "white"
	anchors.margins: 2

	GridLayout{
		columns: 2
		rows: 3
		rowSpacing: 2
		columnSpacing:2
		anchors.fill: parent
		anchors.margins: 2
		clip: true

		Text {
			id:episodeDN
			text: display_name;
			font.pointSize: 14;
			font.bold: true;
			elide: Text.ElideRight
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}

		Text {
			id:episodePubDate
			text: qsTr("Published")+": " + pub_date;
			font.pointSize: 12;
			elide: Text.ElideRight
			Layout.alignment: Qt.Alignleft | Qt.AlignVCenter
		}

		Text {
			id:episodeDuration
			text: qsTr("Duration")+": "+ duration;
			font.pointSize: 12;
			elide: Text.ElideRight
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
		}

		Text {
			id:episodeDescription
			text: description ;
			wrapMode: Text.WordWrap
			font.pointSize: 10
			elide: Text.ElideRight
			Layout.preferredWidth: parent.width-16
			Layout.preferredHeight: parent.heigth*0.5
			Layout.maximumWidth: parent.width-4
			Layout.maximumHeight: parent.heigth*0.6
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}
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


