import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

ListView {
    id: podcastlist;
    property string objectName : "PodcastList";
    width: stackView.width;
    height: stackView.height;
    contentWidth: stackView.width;
    rebound: listBoundTransition;

    delegate: PodcastDelegate{
        id: podcastdelegate
    }

    model: podcastmodel

    Connections {
        onFlickStarted : {
            viewResetTimer.restart();
        }
    }

    PodcastContextMenu{
        id: podcastControl;
    }
}
