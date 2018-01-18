import QtQuick 2.7
import QtQuick.Controls 2.1
import Backend 1.0


ListView {
    id: podcastList
    width: parent.width
    height: parent.height
    contentWidth: parent.width
    focus: true
    boundsBehavior: Flickable.StopAtBounds
    flickableDirection: Flickable.AutoFlickDirection
    delegate: podcastDelegate
    model: podcastmodel
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
}
