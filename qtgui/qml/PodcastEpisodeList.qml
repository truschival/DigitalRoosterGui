import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import ruschi.PodcastEpisodeModel 1.0

ListView {
    width: parent.width
    height: parent.height;

    rebound: listBoundTransition;

    property PodcastEpisodeModel episodemodel;

    delegate:PodcastEpisodeDelegate{
        id: ped
    }

    Connections {
        function onFlickStarted () {
            viewResetTimer.restart();
        }
    }
}
