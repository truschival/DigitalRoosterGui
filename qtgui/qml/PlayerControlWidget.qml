import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0
import "Icon.js" as MdiFont
import "Jsutil.js" as Util

Popup {
	closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
	background: Rectangle {
        color: "#3F51B5"
    }

	enter: Transition {
		NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 ; duration: 300}
	}
	exit: Transition {
		NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 ; duration: 400}
	}

    Timer {
        id: interactiontimer
        interval: 5000
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
		Layout.topMargin: 0;
		anchors.bottomMargin: 12;
		anchors.fill: parent;

		Text{
			id: currentMediaTitle
			text: "" ;
			font: Style.font.label;
			color: "white";
			elide: Text.ElideRight;

			Layout.topMargin: 0;
			Layout.columnSpan: 3;
			Layout.fillWidth: true;
			Layout.alignment: Qt.AlignCenter| Qt.AlignTop
		}

		IconButton {
			id: backwardBtn
			Layout.alignment: Qt.AlignRight| Qt.AlignTop
			Layout.minimumWidth: parent.width/3 - 20;
			Layout.preferredWidth: parent.width/3 ;

			text: MdiFont.Icon.rewind

			onClicked: {
				interactiontimer.restart()
				playerProxy.seek(-10000)
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
			Layout.minimumWidth: parent.width/3 - 20;
			Layout.preferredWidth: parent.width/3 ;

			text: MdiFont.Icon.fastForward
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
			Layout.bottomMargin: 10;
			Layout.topMargin: Style.itemMargins.slim;

			Text {
				id: timeElapsed
				text: Util.display_time_ms(playerProxy.position)
				font: Style.font.valueLabel;
				Layout.alignment: Qt.AlignRight | Qt.AlignTop
				color: "white"
			}

			Slider {
				id: slider
				Layout.fillWidth: true;
				Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
				Layout.topMargin: -10;
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
				font: Style.font.valueLabel;
				color: "white"
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
