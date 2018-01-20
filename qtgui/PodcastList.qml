import QtQuick 2.7
import QtQuick.Controls 2.1

ListView {
    width: parent.width
    height: parent.height
   
    contentWidth: parent.width
    focus: true
    
    delegate: PodcastDelegate{
    	id: podcastdelegate
    }

    model: podcastmodel       
}
