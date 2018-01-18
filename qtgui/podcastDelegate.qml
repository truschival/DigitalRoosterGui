import QtQuick 2.0

Rectangle {
    id: podcastDelegate
    width: 120; height: 100
    color: "blue"
    border.color: "black"

    Text { x: 5; y: 4; text: 'Title: ' + Title }
    Text { x: 5; y: 15; text: 'Url:' + URL }
}

