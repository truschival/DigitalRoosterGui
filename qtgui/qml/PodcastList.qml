// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4

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
        function onFlickStarted() {
            viewResetTimer.restart();
        }
    }

    PodcastContextMenu{
        id: podcastControl;
    }
}
