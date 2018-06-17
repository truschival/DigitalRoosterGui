import QtQuick 2.0
import QtQuick.Layouts 1.3
import ruschi.PodcastEpisodeModel 1.0

Rectangle{
	id: podcastdelegate
	width:  podcastlist.width;
	height: 100;
	color: podcastlist.currentItem == this ? "lightblue" : "white"

	GridLayout{
		columns: 3
		rows: 2
		columnSpacing: 3
		rowSpacing:3
		anchors.margins: 3
		anchors.fill: parent

		Image {
			id: podcasticon
			Layout.maximumWidth: 96
			Layout.maximumHeight: 96
			Layout.rowSpan: 2
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			Layout.fillHeight: true;

			fillMode: Image.PreserveAspectFit
			source: logo_image
		}

		Text {
			height: 28;
			text: display_name ;
			font.pointSize: 15;
			font.bold: true;
			elide: Text.ElideRight
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			Layout.fillWidth: true;
		}

		Text {
			text: episode_count ;
			wrapMode: Text.WordWrap;
			font.pointSize: 11;
			Layout.preferredWidth: (parent.with-podcasticon.width)*0.2
			Layout.alignment: Qt.AlignRight | Qt.AlignTop
		}

		Text {
			text: description ;
			elide: Text.ElideRight;
			wrapMode:  Text.WordWrap;
			font.pointSize: 10;
			Layout.columnSpan: 2;
			Layout.fillWidth: true;
			Layout.maximumHeight: (parent.height-8)/2
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
		}
	}


	MouseArea {
		id: mouseArea
		anchors.fill: parent
		onClicked:  {
			podcastlist.currentIndex =index;

			var comp = Qt.createComponent("qrc:/PodcastEpisodeList.qml")
			if(comp === null){
				console.log("cannot create new PodcastEpisodeList");
				return;
			}
			if( comp.status !== Component.Ready ){
				if( comp.status === Component.Error )
					console.log("Error:"+ comp.errorString() );
			}

			var pel = comp.createObject(stackView,
										{"model":
										 podcastlist.model.get_episodes(index),
										 "episodemodel" : podcastlist.model.get_episodes(index)});
			stackView.push(pel);
		}
	}

}


