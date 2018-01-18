import QtQuick 2.7
import QtQuick.Controls 2.1

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    ListModel {
        id: podcastmodel
        ListElement {
            Title: "Alternativlos"
            URL: "www.alternativlos.de"
        }
        ListElement {
            Title: "BBC World Service"
            URL: "bbc.co.uk"
        }
        ListElement {
            Title: "Arms Control Wonk"
            URL: "acw.org"
        }
    }

    RoundButton {
        text: qsTr("+")
        highlighted: true
        anchors.margins: 10
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onClicked: {
            podcastList.update()
        }
    }
}
