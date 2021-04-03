// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4

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
