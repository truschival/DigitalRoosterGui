import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
/* import QtQuick.Controls.Universal 2.1 */
import Qt.labs.settings 1.0

import "Icon.js" as MdiFont

ApplicationWindow {
	id: applicationWindow
	visible: true
	width: 480
	height: 320
	title: qsTr("DigitalRooster")

	Clock{
		id: currentTime
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
		}
	}

	Drawer {
		id: drawer
		width: Math.min(applicationWindow.width, applicationWindow.height) * 0.4
		height: applicationWindow.height
		interactive: stackView.depth == 1

		ListView {
			id: listView

			focus: true
			currentIndex: -1
			anchors.fill: parent

			delegate: IconButton {
				width: parent.width
				text: model.title
				highlighted: ListView.isCurrentItem

				onClicked: {
					if(listView.currentIndex != index){
						listView.currentIndex = index
						stackView.pop(null)
						stackView.clear()
						gc();
						stackView.push(model.source)
					}
					drawer.close()
				}
			}

			model: ListModel {
				ListElement { title: "\uf150"; source: "qrc:/ClockPage.qml" }
				ListElement { title: "\uf223"; source: "qrc:/PodcastList.qml" }
			}
		}
	}


	StackView {
		id: stackView
		anchors.fill: parent
		initialItem: ClockPage{}
	}
}
