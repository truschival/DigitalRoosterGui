import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0
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
            spacing: 5
            anchors.fill: parent

            IconButton {
                text: MdiFont.Icon.menu
				Layout.margins: 2
                onClicked: {
                    drawer.open()
                }
            }
            Label {
                id: titleLabel
				Layout.margins: 2
                text: currentTime.timestring_lz
                font.pixelSize: 20
                elide: Label.ElideRight
                Layout.fillWidth: true
            }

			 IconButton {
                id : playerControlBtn
                text: MdiFont.Icon.play
				Layout.margins: 2

                onClicked:{
					playerControlWidget.setVisible(true)
                }
            }


			IconButton {
                id : volButton
				Layout.margins: 2
                text: "\uf4c3"
                onClicked:{
					volumeMenu.popup(-width/3,height/3)
				}

				Menu {
					id: volumeMenu
					width: 50
                    height: 180
					Layout.margins: 2

                    Label{
                        text: volumeSlider.value
						anchors.horizontalCenter:parent.horizontalCenter
					}
					Slider {
						id: volumeSlider
						anchors.horizontalCenter:parent.horizontalCenter
                        orientation: Qt.Vertical
                        from: 0
                        to: 100
                        stepSize: 1
						wheelEnabled: true
                        value: playerProxy.volume
                        onMoved: {
                            playerProxy.volume = value;
                        }
					}
				}
            }

            IconButton {
                id : backButton
                text: MdiFont.Icon.keyboardBackspace
				Layout.margins: 2
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
				ListElement { title: "\uf020"; source: "qrc:/AlarmList.qml"; objectName:"AlarmList"; }
            }
        }
    }

	PlayerControlWidget{
		id: playerControlWidget
		anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width*0.8
        height: parent.height*0.3
        anchors.horizontalCenterOffset: 0
        anchors.bottomMargin: 0
        visible: false
        z: 1
        anchors.bottom: parent.bottom
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
