import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0
import ruschi.AbstractMediaPlayer 1.0
import "Icon.js" as MdiFont
import "Jsutil.js" as Util

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 480
    height: 320
    property alias playerControlWidget: playerControlWidget
    property string functionMode: "Clock"

    title: qsTr("DigitalRooster")

    Clock{
        id: currentTime
    }

	FontLoader { 
		id: materialdesignIconsFont;
		source: "materialdesignicons-webfont.ttf" 
	}


    header: ToolBar {
        RowLayout {
            spacing: 10
            anchors.fill: parent

            IconButton {
                text: MdiFont.Icon.menu
                onClicked: {
                    drawer.open()
                }
            }
            Label {
                id: titleLabel
                text: currentTime.timestring_lz
                font.pixelSize: 20
                elide: Label.ElideRight
                Layout.fillWidth: true
            }

            IconButton {
                id : backButton
                text: MdiFont.Icon.keyboardBackspace
                visible: (stackView.depth > 1)

                onClicked:{
                    if (stackView.depth > 1){
                        stackView.pop()
                        console.log("BackButton")
                    }
                }

                Shortcut {
                    sequence: StandardKey.Back
                    onActivated: backButton.onClicked();
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: applicationWindow.width * 0.2
        height: applicationWindow.height
        interactive: true

        ListView {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: IconButton {
                width: parent.width
                text: model.title
                highlighted: listView.currentIndex == index

                onClicked: {
                    if(listView.currentIndex != index){
                        listView.currentIndex = index

                        stackView.pop(null)
                        stackView.push(model.source)
                    }
                    drawer.close()
                }
            }

            model: ListModel {
                ListElement { title: "\uf150"; source: "qrc:/ClockPage.qml";   objectName:"ClockPage"; }
                ListElement { title: "\uf223"; source: "qrc:/PodcastList.qml"; objectName:"PodcastList"; }
				ListElement { title: "\uf43B"; source: "qrc:/IRadioList.qml"; objectName:"InternetRadio"; }
            }
        }

    }

    Rectangle {
        id: playerControlWidget

        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width*0.8
        height: parent.height*0.3
        anchors.horizontalCenterOffset: 0
        anchors.bottomMargin: 0
        visible: false
        z: 1
        anchors.bottom: parent.bottom

        function setVisible(visible){
            interactiontimer.restart()
            playerControlWidget.visible=visible
        }

        Timer {
            id: interactiontimer
            interval: 2500;
            running: true;
            repeat: false;
            onTriggered: parent.setVisible(false)
        }

        IconButton {
            id: playBtn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top : parent.top
            anchors.topMargin: 2
            text: MdiFont.Icon.play // default to play icon

            onClicked: {
                console.log("playBtn")
                interactiontimer.restart()

                if(playerProxy.playbackState == MediaPlayer.PlayingState){
                    playerProxy.pause()
                }
                else{
                    playerProxy.play()
                }
            }

			function switchPlayButtonIcon(playbackState){
                switch (playbackState){
                case MediaPlayer.PlayingState:
                    playBtn.text =  MdiFont.Icon.pause
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
            anchors.leftMargin: 25
            anchors.top: playBtn.top
            text: MdiFont.Icon.fastForward
            onClicked: {
                interactiontimer.restart()
                playerProxy.seek(5000)
            }
        }

        IconButton {
            id: backwardBtn
            anchors.right: playBtn.left
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
            width: parent.width*0.85
            anchors.top: playBtn.bottom
            anchors.topMargin: -15
            enabled: playerProxy.seekable
			visible: playerProxy.seekable

            onValueChanged: {
                interactiontimer.restart()
            }
            onMoved:{
                playerProxy.set_position(value* playerProxy.duration)
            }
        }

        Text{
            id: timeElapsed
            text: Util.display_time_ms(playerProxy.position)
            anchors.horizontalCenter: slider.left
            anchors.top: slider.bottom
            anchors.margins: 2
        }
        Text{
            id: durationTotal
			text: Util.display_time_ms(playerProxy.duration)
            anchors.horizontalCenter: slider.right
            anchors.top: slider.bottom
            anchors.margins: 2
        }

		function updatePosition(pos){
			slider.value = pos/playerProxy.duration
			timeElapsed.text=Util.display_time_ms(pos)
		}

		/***********************************************************************/
		Component.onCompleted : {
			playerProxy.playback_state_changed.connect(playBtn.switchPlayButtonIcon)
			playerProxy.position_changed.connect(updatePosition)
		}
    }


    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: ClockPage{
            id:initalClockPage
            property string objectName : "InitialPage"
        }
    }
}
