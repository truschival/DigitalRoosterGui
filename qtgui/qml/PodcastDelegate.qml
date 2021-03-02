import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import ruschi.PodcastEpisodeModel 1.0
import QtQuick.Controls.Material 2.1

Rectangle{
    id: podcastdelegate
    width:  Style.contentWidth
    height: DEFAULT_ICON_WIDTH+Style.itemMargins.slim+2;
    radius: 2;
    border.width: 1;
    color: Material.background;

    GridLayout{
        anchors.margins: Style.itemMargins.slim;
        anchors.fill: parent
        columns: 3
        rows: 2
        columnSpacing: Style.itemSpacings.medium;
        rowSpacing:Style.itemSpacings.dense;

        Image {
            id: podcasticon;
            Layout.minimumHeight: DEFAULT_ICON_WIDTH;
            Layout.minimumWidth: DEFAULT_ICON_WIDTH;
            Layout.maximumHeight: DEFAULT_ICON_WIDTH;
            Layout.maximumWidth : DEFAULT_ICON_WIDTH;
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignLeft| Qt.AlignVCenter
            Layout.leftMargin: 1;
            fillMode: Image.Pad
            source: logo_image
        }

        Text {
            text: display_name;
            font: Style.font.listItemHead;
            elide: Text.ElideRight;
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            Layout.fillWidth: true;
            color: "white";
        }

        Text {
            text: episode_count;
            wrapMode: Text.WordWrap;
            font: Style.font.valueLabel;
            Layout.preferredWidth: (parent.with-podcasticon.width)*0.2
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
            color: "white";
        }

        Text {
            text: description ;
            elide: Text.ElideRight;
            wrapMode: Text.WordWrap;
            font: Style.font.flowText;
            lineHeight: 0.8;
            Layout.topMargin: 0;
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            Layout.maximumHeight: parent.height/1.8
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            color: "white";
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        /* Simple click (or press for what it's worth) shows the episodes */
        onClicked:  {
            podcastlist.currentIndex =index;

            var comp = Qt.createComponent("qrc:/PodcastEpisodeList.qml")
            if(comp === null){
                console.log("cannot create new PodcastEpisodeList");
                return;
            }
            if( comp.status !== Component.Ready ){
                if( comp.status === Component.Error )
                    console.log("Error:"+ comp.errorString() );
            }

            var pel = comp.createObject(stackView,
                                        {"model":
                                         podcastlist.model.get_episodes(index),
                                         "episodemodel" : podcastlist.model.get_episodes(index)});
            stackView.push(pel);
            // Enter Podcast List, give user time to browse
            viewResetTimer.restart();
        }

        onPressAndHold:  {
            podcastlist.currentIndex = index;
            podcastControl.title = display_name ;
            podcastControl.popup((applicationWindow.width- podcastControl.width)/2,
                                 (applicationWindow.height- podcastControl.height)/2 - Style.itemMargins.extrawide)
        }
    }
}
