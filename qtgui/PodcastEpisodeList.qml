import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import ruschi.PodcastEpisodeModel 1.0

ListView {
	width: parent.width
	height: parent.height

	property PodcastEpisodeModel foo;

	delegate:PodcastEpisodeDelegate{
		id: ped
	}
}
