import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1


ListView {
	// not static id or model - we have many
    //id : lv
    //model: parent.model
	width: parent.width
	height: parent.height

	delegate:PodcastEpisodeDelegate{
		id: ped
	}
}
