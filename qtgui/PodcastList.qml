import QtQuick 2.7
import QtQuick.Controls 2.1

ListView {
    width: parent.width
    height: parent.height
    antialiasing: true
    highlightRangeMode: ListView.ApplyRange
    boundsBehavior: Flickable.StopAtBounds
    maximumFlickVelocity: 1500
    flickDeceleration: 1800

    contentWidth: parent.width
    focus: true
    
    delegate: PodcastDelegate{
    	id: podcastdelegate
    }

    model: podcastmodel
    onCurrentItemChanged: console.log(model.get(podcastlist.currentIndex).name + ' selected')       
}
