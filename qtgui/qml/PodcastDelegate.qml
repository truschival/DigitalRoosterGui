import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import ruschi.PodcastEpisodeModel 1.0

Rectangle{
    id: podcastdelegate
    width:  Style.contentWidth;
    height: Style.contentHeight*0.45;
    radius: 3;
    border.width: 1;
    color: podcastlist.currentItem == this ?
	Style.colors.selected : Style.colors.unselected ;

    GridLayout{
	anchors.margins: Style.itemMargins.slim;
	anchors.fill: parent
		columns: 3
		rows: 2
		columnSpacing: Style.itemSpacings.medium;
		rowSpacing:Style.itemSpacings.dense;

		Image {
			id: podcasticon;
			Layout.maximumHeight : podcastdelegate.height-
				2*Style.itemMargins.slim;
			Layout.maximumWidth :  podcastdelegate.height-
				2*Style.itemMargins.slim;
			// yes width is specified as height,
			// gives the layout a hint while image is not loaded
			Layout.minimumWidth :  0.9*podcastdelegate.height-
				2*Style.itemMargins.slim;
			Layout.minimumHeight : 0.9*podcastdelegate.height-
				2*Style.itemMargins.slim;

			Layout.rowSpan: 2
			Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter

			fillMode: Image.PreserveAspectFit
			source: logo_image
		}

		Text {
			text: display_name;
			font: Style.font.listItemHead;
			elide: Text.ElideRight;
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			Layout.fillWidth: true;
		}

		Text {
			text: episode_count;
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
			Layout.topMargin: 0;
			Layout.columnSpan: 2;
			Layout.fillWidth: true;
			Layout.maximumHeight: parent.height/2
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
		}
	}


    MouseArea {
	id: mouseArea
	anchors.fill: parent

	/* Simple click (or press for what it's worth) shows the episodes */
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

	onPressAndHold:  {
	    podcastlist.currentIndex = index;
	    console.log("Podcast pressed: " + index)
	    podcastControl.popup((applicationWindow.width- podcastControl.width)/2,
				 (applicationWindow.height- podcastControl.height)/2)
	}
    }

}
