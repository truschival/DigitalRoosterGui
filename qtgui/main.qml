
import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import Qt.labs.settings 1.0

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 480
    height: 320
    title: qsTr("DigitalRooster")


    header: ToolBar {
        Material.foreground: "white"

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                text: qsTr("select")
                onClicked: {
                	drawer.open()
                }
            }

            Label {
                id: titleLabel
                text: "SomeLabel"
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(applicationWindow.width, applicationWindow.height) / 3
        height: applicationWindow.height
        interactive: stackView.depth == 1

        ListView {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                highlighted: ListView.isCurrentItem
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
                ListElement { title: "Clock"; source: "qrc:/ClockPage.qml" }
                ListElement { title: "Podcasts"; source: "qrc:/PodcastList.qml" }
            }
        }
    }


    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: ClockPage{}
    }
}
