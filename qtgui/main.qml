import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
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

	property PodcastEpisode currentEpisode

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

        IconButton {
            id: playBtn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top : parent.top
            anchors.topMargin: 2

            text: parent.playing? MdiFont.Icon.play: MdiFont.Icon.pause
            onClicked: {
                playerControlWidget.playing = !playerControlWidget.playing
                interactiontimer.restart()
                console.log("playBtn")
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
            value: 0.6
            onValueChanged: interactiontimer.restart()
        }
        Text{
            id: timeElapsed
            text: "1:13:00"
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
