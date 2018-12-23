import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import ruschi.PodcastEpisodeModel 1.0

Rectangle{
	id: podcastdelegate
	width:  Style.contentWidth;
	height: Style.contentHeight*0.4;
	radius: 3;
	border.width: 1;
	color: podcastlist.currentItem == this ? "#2196F3" : "LightGrey" ;

	GridLayout{
		columns: 3
		rows: 2
		columnSpacing: Style.itemSpacings.medium;
		rowSpacing:Style.itemSpacings.dense;
		anchors.margins: Style.itemMargins.slim;
		anchors.fill: parent

		Image {
			id: podcasticon;
			Layout.maximumHeight : podcastdelegate.height-2*Style.itemMargins.slim
			Layout.minimumHeight : podcastdelegate.height-2*Style.itemMargins.slim
			// yes width is specified as height, gives the layout a hint while image is not loaded
			Layout.maximumWidth : podcastdelegate.height-2*Style.itemMargins.slim
			Layout.minimumWidth : podcastdelegate.height-2*Style.itemMargins.slim

			Layout.rowSpan: 2
			Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter

			fillMode: Image.PreserveAspectFit
			source: logo_image
		}

		Text {
			text: display_name ;
			font: Style.font.listItemHead;
			elide: Text.ElideRight
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			Layout.fillWidth: true;
		}

		Text {
			text: episode_count ;
			wrapMode: Text.WordWrap;
			font: Style.font.valueLabel;
			Layout.preferredWidth: (parent.with-podcasticon.width)*0.2
			Layout.alignment: Qt.AlignRight | Qt.AlignTop
		}

		Text {
			text: description ;
			elide: Text.ElideRight;
			wrapMode:  Text.WordWrap;
			font: Style.font.flowText;
			lineHeight : 0.8;
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
