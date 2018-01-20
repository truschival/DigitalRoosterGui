import QtQuick 2.7
import QtQuick.Controls 2.1

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("DigitalRooster")

	PodcastList{
		 id: podcastlist
	}

    RoundButton {
    	id: uselessbutton
        text: qsTr("+")
        highlighted: true
        anchors.margins: 10
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        
        onClicked: {
            podcastList.update()
            uselessbutton.highlighted = false
        }
    }
}
