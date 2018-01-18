import QtQuick 2.7
import QtQuick.Controls 2.1

ListView {
    width: parent.width
    height: parent.height
    
    contentWidth: parent.width
    
    focus: true
    boundsBehavior: Flickable.StopAtBounds
    flickableDirection: Flickable.AutoFlickDirection
    
    delegate: PodcastDelegate{
    	id: podcastdelegate
    }

    model: PodcastModel{
    	 id: podcastmodel
    }
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
}
