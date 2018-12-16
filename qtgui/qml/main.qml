import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "Icon.js" as MdiFont
import "Jsutil.js" as Util


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: Style.canvasWidth;
    height: Style.canvasHeight;

	property alias playerControlWidget: playerControlWidget
    property string functionMode: "Clock"

    Clock{
        id: currentTime
    }

	FontLoader {
		id: materialdesignIconsFont;
		source: "materialdesignicons-webfont.ttf"
	}

    header: ToolBar {
		height: Style.toolbarHeight;
		
        RowLayout {
            spacing: Style.itemSpacings.dense;
            anchors.fill: parent
            Layout.margins: Style.itemMargins.slim;

            IconButton {
                text: MdiFont.Icon.menu
				Layout.leftMargin: Style.itemMargins.medium;
				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw

                onClicked: {
                    drawer.open()
                }
            }
            Label {
                id: titleLabel
                text: (stackView.depth > 1) ? currentTime.timestring_lz : qsTr("DigitalRooster");
                font: Style.h1Font;
                elide: Label.ElideRight
                Layout.fillWidth: true
            }
				
			IconButton {
                id : playerControlBtn
                text: MdiFont.Icon.play
				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw
				
                onClicked:{
					playerControlWidget.setVisible(true)
                }
            }

			IconButton {
                id : volButton
                text: "\uf4c3"
				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw
			
                onClicked:{
					volumePopUp.show();
				}
            }
            IconButton {
                id : backButton
                text: MdiFont.Icon.keyboardBackspace

				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw

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
        width: Style.drawer.w;
        height: applicationWindow.height
        interactive: true;
		edge: Qt.LeftEdge;

        ListView {
            id: listView
			anchors.fill: parent
			spacing: Style.itemSpacings.dense;
			anchors.margins: Style.itemMargins.slim;
			anchors.horizontalCenter: parent.horizontalCenter

            focus: true;
            currentIndex: -1;
		
            delegate: IconButton {
                width:  Style.buttons.pw;
				height: Style.buttons.ph;
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
                    if(index === listView.count -1 ){
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
			spacing: Style.itemSpacings.medium;
			IconButton {
				id: poweroffBtn
				text: "\uf901"
				Layout.minimumHeight: Style.buttons.ph;
				Layout.minimumWidth: Style.buttons.pw;
				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw;
				Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
				onClicked: {
					console.log("power off button")
					powerControl.power_off();
				}
			}

			IconButton {
				id: standbyBtn
				text: "\uf903";
				Layout.minimumHeight: Style.buttons.ph;
				Layout.minimumWidth: Style.buttons.pw;
				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw;
				Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
				onClicked: {
					console.log("standby button")
					powerControl.toggle_power_state();
				}
			}
			
			IconButton {
				id: rebootBtn
				text: "\uf900";
				Layout.minimumHeight: Style.buttons.ph;
				Layout.minimumWidth: Style.buttons.pw;
				Layout.preferredHeight: Style.buttons.ph;
				Layout.preferredWidth: Style.buttons.pw;
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
        width: parent.width*0.8;
        height: parent.height*0.4;
        anchors.horizontalCenterOffset: 0
        anchors.bottomMargin: 15
        visible: false
        z: 1
        anchors.bottom: parent.bottom
	}

	VolumePopup{
		id: volumePopUp
		x: Math.round((applicationWindow.width - width) / 2)
		y: Math.round((applicationWindow.height - height) / 2)
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
