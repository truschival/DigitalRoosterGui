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
            Layout.margins: 2

            IconButton {
                text: MdiFont.Icon.menu
				Layout.leftMargin: 4
				Layout.preferredHeight: 58;
				Layout.preferredWidth: 58;

                onClicked: {
                    drawer.open()
                }
            }
            Label {
                id: titleLabel
                text: (stackView.depth > 1) ? currentTime.timestring_lz : qsTr("DigitalRooster");
                font.pixelSize: 20
                elide: Label.ElideRight
                Layout.fillWidth: true
            }

			IconButton {
                id : playerControlBtn
                text: MdiFont.Icon.play
				Layout.preferredHeight: 58;
				Layout.preferredWidth: 58;

                onClicked:{
					playerControlWidget.setVisible(true)
                }
            }


			IconButton {
                id : volButton
                text: "\uf4c3"
                Layout.preferredHeight: 58;
				Layout.preferredWidth: 58;
				Layout.leftMargin: 4
				Layout.rightMargin: 4

                onClicked:{
					volumeMenu.popup(-width/3,height/3)
				}

				Menu {
					id: volumeMenu
					width: 58
                    height: applicationWindow.height*0.8

                    Label{
                        font.pointSize: 24
						font.weight: Font.DemiBold

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

				Layout.preferredHeight: 58;
				Layout.preferredWidth: 58;
				Layout.rightMargin: 4
                visible: (stackView.depth > 1)

                onClicked:{
					stackView.backNavigate()
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
        width: 66
        height: applicationWindow.height
        interactive: true;
		edge: Qt.LeftEdge;
		dragMargin: 20;

        ListView {
            id: listView
			anchors.fill: parent
            focus: true
            currentIndex: -1
			spacing: 2
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.margins: 4

            delegate: IconButton {
                width: 58
				height: 58
                text: model.title
                highlighted: listView.currentIndex == index

                onClicked: {
					console.log("Current "+ listView.currentIndex + " index: "+index + " depth:"+ stackView.depth)
					if( stackView.depth > 1){
						stackView.pop(null)
					}
					listView.currentIndex = index
                    drawer.close()
                    /* Special item: power off button */
                    if(index === listView.count-1){
                    	console.log("last item!")
                    	powerOffMenu.popup((applicationWindow.width-powerOffMenu.width)/2,
										   (applicationWindow.height-powerOffMenu.height)/2)
                    	return; // nothing to do
                    }
					stackView.push(model.source)
                }
            }

            model: ListModel {
                ListElement { title: "\uf223"; source: "qrc:/PodcastList.qml"; objectName:"PodcastList"; }
				ListElement { title: "\uf43B"; source: "qrc:/IRadioList.qml"; objectName:"InternetRadio"; }
				ListElement { title: "\uf020"; source: "qrc:/AlarmList.qml"; objectName:"AlarmList"; }
				ListElement { title: "\uf62e"; source: "qrc:/SettingsPage.qml"; objectName:"Settings"; }
				ListElement { title: "\uf425"; source: ""; objectName:"PowerOff"; }
            }
        }
    }

	Menu {
		id: powerOffMenu
		RowLayout {
			spacing: 4
			IconButton {
				id: poweroffBtn
				text: "\uf425"
				Layout.minimumHeight: 58;
				Layout.minimumWidth: 58;
				Layout.preferredHeight: 58;
				Layout.preferredWidth: 58;
				Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
				onClicked: {
					console.log("power off button")
					powerControl.power_off();
				}
			}

			IconButton {
				id: rebootBtn
				text: "\uf426";
				Layout.minimumHeight: 58;
				Layout.minimumWidth: 58;
				Layout.preferredHeight: 58;
				Layout.preferredWidth: 58;
				Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
				onClicked: {
					console.log("reboot button")
					powerControl.reboot();
				}
			}
		}
	}

	PlayerControlWidget{
		id: playerControlWidget
		anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width*0.8
        height: 100
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

		function backNavigate(){
			if (stackView.depth > 1){
				stackView.pop()
			} else{
				stackView.currentIndex = -1;
			}
		}
    }
}
