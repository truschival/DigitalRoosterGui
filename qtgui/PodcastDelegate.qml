import QtQuick 2.0

Component {
	Rectangle{
	    width: podcastlist.width; 
	    height: podcastlist.height/2;
	    color: ListView.isCurrentItem ? "lightblue" : "white"
	    border.color: "black"
	
	    Text { x: 5; y: 4; text:  'Title: ' + display_name }
	    Text { x: 5; y: 15; text: 'Url:' + link }
	}
}

