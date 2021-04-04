// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtMultimedia 5.9

import ruschi.PodcastEpisode 1.0
import "Jsutil.js" as Util

Popup {
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    Timer {
        id: interactiontimer
        interval: 4000
        running: true
        repeat: false
        onTriggered: playerControlWidget.close();
    }

    GridLayout{
        columns: 3;
        rows: 3;
        columnSpacing: Style.itemSpacings.medium;
        rowSpacing: Style.itemSpacings.dense;
        anchors.margins: 0;
        anchors.fill: parent;

        Text{
            id: currentMediaTitle
            text: "" ;
            font: Style.font.label;
            color: Style.colors.selected;
            elide: Text.ElideRight;

            Layout.topMargin: 0;
            Layout.columnSpan: 3;
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignCenter | Qt.AlignTop
        }

        IconButton {
            id: backwardBtn
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
            Layout.minimumWidth: parent.width/3 - 20;
            Layout.preferredWidth: parent.width/3 ;
            text: "\uf45f";
            onClicked: {
                interactiontimer.restart()
                playerProxy.seek(-10000)
            }
        }

        IconButton {
            id: playBtn
            Layout.alignment: Qt.AlignCenter| Qt.AlignTop
            Layout.fillWidth: true;
            text: "\uf40a" // default to play icon

            onClicked: {
                interactiontimer.restart()
                if (playerProxy.playbackState === MediaPlayer.PlayingState) {
                    playerProxy.pause()
                } else {
                    playerProxy.play()
                }
            }

            function switchPlayButtonIcon(playbackState) {
                switch (playbackState) {
                case MediaPlayer.PlayingState:
                    playBtn.text = "\uf3e4"
                    break
                case MediaPlayer.PausedState:
                    playBtn.text = "\uf40a"
                    break
                case MediaPlayer.StoppedState:
                    playBtn.text = "\uf40a"
                    break
                default:
                    console.log("player???")
                }
            }
        }

        IconButton {
            id: forwardBtn
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            Layout.minimumWidth: parent.width/3 - 20;
            Layout.preferredWidth: parent.width/3 ;

            text: "\uf211"
            onClicked: {
                interactiontimer.restart()
                playerProxy.seek(10000)
            }
        }
        //Row 3
        RowLayout{
            Layout.columnSpan: 3;
            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignCenter| Qt.AlignTop
            Layout.bottomMargin: 0;
            Layout.topMargin: Style.itemMargins.slim;

            Text {
                id: timeElapsed
                text: Util.display_time_ms(playerProxy.position)
                font: Style.font.valueLabel;
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                color: Style.colors.primaryText;
            }

            Slider {
                id: slider
                Layout.fillWidth: true;
                Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                Layout.topMargin: -15;
                enabled: playerProxy.seekable

                onMoved: {
                    playerProxy.set_position(value * playerProxy.duration)
                    interactiontimer.restart()
                }
            }
            Text {
                id: durationTotal
                text: playerProxy.seekable? Util.display_time_ms(playerProxy.duration): "\u221E"
                Layout.preferredWidth: timeElapsed.width
                font: playerProxy.seekable? Style.font.valueLabel: Style.font.listItemHeadListened;
                color: Style.colors.primaryText;
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            }
        }// RowLayout Row 3
    }//Gridlayout

    /***********************************************************************/
    function show(visible) {
        interactiontimer.start();
        playerControlWidget.open();
    }

    function updatePosition(pos) {
        slider.value = pos / playerProxy.duration
        timeElapsed.text = Util.display_time_ms(pos)
    }

    function setCurrentMediaTitle(title) {
        currentMediaTitle.text = title;
    }
    /***********************************************************************/
    Component.onCompleted: {
        playerProxy.playback_state_changed.connect(playBtn.switchPlayButtonIcon)
        playerProxy.position_changed.connect(updatePosition)
    }
}
