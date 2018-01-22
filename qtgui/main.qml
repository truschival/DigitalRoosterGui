import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("DigitalRooster")

	PodcastList{
		id: podcastlist
	}
}
