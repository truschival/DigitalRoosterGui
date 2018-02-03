import QtQuick 2.0

Rectangle{
	width:  parent.width;
	height: 100;

	Text {
		id:dn
		text: display_name ;

		width: parent.width-10;
		height: 28;
		anchors.top: parent.top;
		anchors.topMargin: 2;
		font.pointSize: 15;
		font.bold: true;
		elide: Text.ElideRight
	}

	Text {
		text: description ;

		width: parent.width-40;
		height: 28;
		anchors.top: dn.bottom;
		anchors.topMargin: 2;
		font.pointSize: 12;
		elide: Text.ElideRight
	}
	Text {
		text: length ;

		width: parent.width-10;
		height: 28;
		anchors.top: dn.bottom;
		anchors.topMargin: 2;
		anchors.right: parent.right;
		anchors.rightMargin: 2;
		font.pointSize: 12;
		elide: Text.ElideRight
	}

	MouseArea {
		anchors.fill: parent
		onClicked:  {
			console.log("PodcastEpisodeDelegate.onClicked()" + display_name + ' -> ' +  index)
		}
	}

}


