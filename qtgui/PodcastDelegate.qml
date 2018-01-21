import QtQuick 2.0

Component {
	Rectangle{
	    width: podcastlist.width; 
	    height: podcastlist.height/3;
	    color: ListView.isCurrentItem ? "lightblue" : "white"
	    border.color: "black"
		Image {
			 width:64
			 height: 64
   			 source: "http://alternativlos.org/squarelogo.png"
		}
		
	    Text { x: 5; y: 4; text:  'Title: ' + display_name }
	    Text { x: 5; y: 15; text: 'Url:' + link }
	    Text { x: 100; y: 35; text: 'Count:' + episode_count }
	}
}

