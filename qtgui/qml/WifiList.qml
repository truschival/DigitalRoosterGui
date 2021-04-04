// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4

ListView {
    boundsBehavior: Flickable.StopAtBounds
    rebound: listBoundTransition;

    delegate: WifiNetworkDelegate{
        id: wifidelegate
    }
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }

    model: wifilistmodel;
}
