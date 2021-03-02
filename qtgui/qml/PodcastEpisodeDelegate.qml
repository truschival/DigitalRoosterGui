import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtMultimedia 5.9
import QtQuick.Controls.Material 2.1

import ruschi.PodcastEpisode 1.0

Rectangle{
    id: rectangle
    width:  stackView.width-2;
    height: 100;
    radius: 3;
    border.width: 1;
    color: Material.background;
    anchors.margins: Style.itemMargins.slim;

    GridLayout{
        columns: 2
        rows: 3
        rowSpacing: Style.itemSpacings.dense;
        columnSpacing: Style.itemSpacings.dense;
        anchors.fill: parent
        anchors.margins: Style.itemMargins.slim;
        clip: true

        Text {
            id:episodeDN
            text: title;
            font: listened ?
                Style.font.listItemHeadListened : Style.font.listItemHead;
            Layout.fillWidth: true
            elide: Text.ElideRight
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.bottomMargin: Style.itemMargins.slim;
            color: "white";
        }

        Text {
            id:episodePubDate
            text: pub_date;
            font: listened ?  Style.font.valueLabel : Style.font.label;
            elide: Text.ElideRight
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.topMargin: Style.itemMargins.slim;
            color: "white";
        }

        Text {
            id: episodeDuration
            text: duration+"h";
            font: listened ?  Style.font.valueLabel : Style.font.label;
            elide: Text.ElideRight
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.topMargin: Style.itemMargins.slim;
            color: "white";
        }

        Text {
            id:episodeDescription
            text: description ;
            wrapMode: Text.WordWrap
            font: Style.font.flowText;
            lineHeight : 0.8;
            elide: Text.ElideRight
            Layout.preferredWidth: parent.width-16
            Layout.preferredHeight: parent.heigth*0.5
            Layout.maximumWidth: parent.width-4
            Layout.maximumHeight: parent.heigth*0.6
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            color: "white";
        }
    }
    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: applicationWindow.pressAndHoldInterval;

        onDoubleClicked:{
            episodemodel.currentIndex = index;
        }

        onPressAndHold:  {
            episodemodel.currentIndex = index;
            episodemodel.send_to_player(index)
            playerControlWidget.setCurrentMediaTitle(title)
            playerControlWidget.show()
        }
    }
}
