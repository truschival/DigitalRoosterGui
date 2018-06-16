import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0
import "Icon.js" as MdiFont
import "Jsutil.js" as Util

Rectangle {
    width: 300
    height: 120
    anchors.horizontalCenterOffset: 0
    anchors.bottomMargin: 0
    visible: false
	radius: 3
    z: 1

	gradient: Gradient {
        GradientStop { position: 0.2; color: "#3F51B5" }
		GradientStop { position: 1.0; color: "#673AB7" }
    }

    Timer {
        id: interactiontimer
        interval: 2500
        running: true
        repeat: false
        onTriggered: parent.setVisible(false)
    }

    IconButton {
        id: playBtn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 2
        text: MdiFont.Icon.play // default to play icon

        onClicked: {
            interactiontimer.restart()
            if (playerProxy.playbackState == MediaPlayer.PlayingState) {
                playerProxy.pause()
            } else {
                playerProxy.play()
            }
        }

        function switchPlayButtonIcon(playbackState) {
            switch (playbackState) {
            case MediaPlayer.PlayingState:
                playBtn.text = MdiFont.Icon.pause
                break
            case MediaPlayer.PausedState:
                playBtn.text = MdiFont.Icon.play
                break
            case MediaPlayer.StoppedState:
                playBtn.text = MdiFont.Icon.play
                break
            default:
                console.log("player???")
            }
        }
    }

    IconButton {
        id: forwardBtn
        anchors.left: playBtn.right
        anchors.top: playBtn.top
        anchors.leftMargin: 25

        text: MdiFont.Icon.fastForward
        onClicked: {
            interactiontimer.restart()
            playerProxy.seek(5000)
        }
    }

    IconButton {
        id: backwardBtn
        anchors.right: playBtn.left
        anchors.top: playBtn.top
        anchors.rightMargin: 25

        text: MdiFont.Icon.rewind
        onClicked: {
            interactiontimer.restart()
            playerProxy.seek(-5000)
        }
    }

    Slider {
        id: slider
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.6
        anchors.top: playBtn.bottom
        anchors.topMargin: -15
        enabled: playerProxy.seekable
        visible: playerProxy.seekable

        onValueChanged: {
            interactiontimer.restart()
        }
        onMoved: {
            playerProxy.set_position(value * playerProxy.duration)
        }
    }

    Text {
        id: timeElapsed
        text: Util.display_time_ms(playerProxy.position)
        anchors.right: slider.left
        anchors.top: playBtn.bottom
        anchors.margins: 2
		visible: playerProxy.seekable
    }
    Text {
        id: durationTotal
        text: Util.display_time_ms(playerProxy.duration)
        anchors.left: slider.right
        anchors.top: playBtn.bottom
        anchors.margins: 2
		visible: playerProxy.seekable
    }


    /***********************************************************************/
    function setVisible(visible) {
        interactiontimer.restart()
        playerControlWidget.visible = visible
    }

    function updatePosition(pos) {
        slider.value = pos / playerProxy.duration
        timeElapsed.text = Util.display_time_ms(pos)
    }

    /***********************************************************************/
    Component.onCompleted: {
        playerProxy.playback_state_changed.connect(playBtn.switchPlayButtonIcon)
        playerProxy.position_changed.connect(updatePosition)
    }
}
