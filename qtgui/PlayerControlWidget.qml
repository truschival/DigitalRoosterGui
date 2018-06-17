import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0
import "Icon.js" as MdiFont
import "Jsutil.js" as Util

Rectangle {
    width: 300
    height: 130
    anchors.horizontalCenterOffset: 0
    visible: false
	radius: 0
    z: 1

	color: "#3F51B5"

    Timer {
        id: interactiontimer
        interval: 2500
        running: true
        repeat: false
        onTriggered: parent.setVisible(false)
    }

	GridLayout{
		columns: 3;
		rows: 3;
		columnSpacing:3
		rowSpacing:0
		anchors.margins: 3
		anchors.fill: parent
		
		Text{
			id: currentMediaTitle
			text: "" ;
			font.pointSize: 10;
			font.bold: true;
			color: "white";
			elide: Text.ElideRight;
			
			Layout.columnSpan: 3;
			Layout.fillWidth: true;
			Layout.alignment: Qt.AlignCenter| Qt.AlignTop
			Layout.bottomMargin: 3
		}

		IconButton {
			id: backwardBtn
			Layout.alignment: Qt.AlignRight| Qt.AlignTop
			Layout.fillWidth: true;

			text: MdiFont.Icon.rewind
			onClicked: {
				interactiontimer.restart()
				playerProxy.seek(-5000)
			}
		}

		IconButton {
			id: playBtn
			Layout.alignment: Qt.AlignCenter| Qt.AlignTop
			Layout.fillWidth: true;
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
			Layout.alignment: Qt.AlignLeft| Qt.AlignTop
			Layout.fillWidth: true;

			text: MdiFont.Icon.fastForward
			onClicked: {
				interactiontimer.restart()
				playerProxy.seek(5000)
			}
		}
		//Row 3
		Text {
			id: timeElapsed
			text: Util.display_time_ms(playerProxy.position)
			Layout.alignment: Qt.AlignCenter|Qt.AlignTop
			color: "white"
		}
		
		Slider {
			id: slider
			Layout.fillWidth: true;
			Layout.alignment: Qt.AlignCenter|Qt.AlignTop
			Layout.topMargin: -15
			enabled: playerProxy.seekable
			
			onValueChanged: {
				interactiontimer.restart()
			}
			onMoved: {
				playerProxy.set_position(value * playerProxy.duration)
			}
		}	
		Text {
			id: durationTotal
			text: playerProxy.seekable? Util.display_time_ms(playerProxy.duration): "\u221E"
			color: "white"
			
			Layout.alignment: Qt.AlignCenter|Qt.AlignTop
			//visible: playerProxy.seekable
		}
		
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

	function setCurrentMediaTitle(title) {
		currentMediaTitle.text = title;
    }
    /***********************************************************************/
    Component.onCompleted: {
        playerProxy.playback_state_changed.connect(playBtn.switchPlayButtonIcon)
        playerProxy.position_changed.connect(updatePosition)
    }
}
