import QtQuick 2.0
import ruschi.PodcastEpisodeModel 1.0

Rectangle{
	id: podcastdelegate
	width:  podcastlist.width;
	height: 100;
    color: podcastlist.currentItem ===this ? "lightblue" : "white"
	Image {
		width:96
		height: 96
		anchors.left: parent.left
		anchors.leftMargin: 2
		anchors.top: parent.top
		anchors.topMargin: 2
		fillMode: Image.PreserveAspectFit
		source: logo_image
	}

	Text { x: 104; width: 464; height: 28; text: display_name ;
		   anchors.top: parent.top; anchors.topMargin: 2;
		   font.pointSize: 15; font.bold: true;elide: Text.ElideRight }
	Text { x: 104; y: 34; width: 464; height: 58; text: description ;
		   elide: Text.ElideRight; wrapMode:
		   Text.WordWrap;font.pointSize: 10 }
	Text { width: 64; height: 64; text: episode_count ;
		   wrapMode: Text.WordWrap; font.pointSize: 11;
		   anchors.top:       parent.top; 
		   anchors.topMargin: 2;
		   anchors.right:      parent.right; 
		   anchors.rightMargin: 2}

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


