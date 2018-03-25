import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtMultimedia 5.8
/* import QtQuick.Controls.Universal 2.1 */
import Qt.labs.settings 1.0

import ruschi.PodcastEpisode 1.0

import "Icon.js" as MdiFont

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

	Dialog{
		id: episodeInfoDialog
		Label{
			text: "AT"
		}
	}


    header: ToolBar {
        Material.foreground: "white"
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
                    console.log("CurrentItem :" + stackView.currentItem.objectName)
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
                        console.log("CurrentItem :" + stackView.currentItem.objectName)
                    }
                    drawer.close()
                }

				

            }

            model: ListModel {
                ListElement { title: "\uf150"; source: "qrc:/ClockPage.qml";   objectName:"ClockPage"; }
                ListElement { title: "\uf223"; source: "qrc:/PodcastList.qml"; objectName:"PodcastList"; }
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
        z: 1
        anchors.bottom: parent.bottom

        property bool playing: false
		property PodcastEpisode currentEpisode;

		MediaPlayer {
			id: player
		}

		function updateEpisode(newEpisode){
			if(newEpisode == null){
				console.log("newEpisode is null")
			}
			if( currentEpisode != null){
				console.log("updating previous episode "+ currentEpisode.display_name)
			}
			currentEpisode = newEpisode
			player.source = newEpisode.url
			player.play()
			playing = true
			durationTotal.text = player.duration
			setVisible(true)
			console.log("currentEpisode is set to "+ currentEpisode.display_name)
		}

        function setVisible(visible){
            console.log("setVisible"+visible)
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


		 function updateSlider(){
			console.log("updating slider position "+player.position )
			slider.value = player.position/player.duration
			timeElapsed.text = player.position
		 }

		 Timer {
            id: sliderUpdateTimer
            interval: 500;
            running: true;
            repeat: true;
            onTriggered: parent.updateSlider()
         }

        IconButton {
            id: playBtn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top : parent.top
            anchors.topMargin: 2

            text: parent.playing? MdiFont.Icon.play: MdiFont.Icon.pause
            onClicked: {
                console.log("playBtn")
				playerControlWidget.playing = !playerControlWidget.playing
                interactiontimer.restart()
				if(playerControlWidget.playing){
					player.pause()
				}
				else{
					player.play()
					console.log("current pos: "+player.position)
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
                console.log("forwardBtn")
            }
        }

        IconButton {
            id: backwardBtn
            anchors.right: playBtn.left
            anchors.rightMargin: 25

            text: MdiFont.Icon.rewind
            onClicked: {
                console.log("backwardBtn")
                interactiontimer.restart()
            }
        }



        Slider {
            id: slider
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width*0.85
            anchors.top: playBtn.bottom
            anchors.topMargin: -15
            value: player.position/player.duration
            onValueChanged: interactiontimer.restart()
        }
        Text{
            id: timeElapsed
            text: player.position
            anchors.horizontalCenter: slider.left
            anchors.top: slider.bottom
            anchors.margins: 2
        }
        Text{
            id: durationTotal
            text: "4:13:00"
            anchors.horizontalCenter: slider.right
            anchors.top: slider.bottom
            anchors.margins: 2
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
