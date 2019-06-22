/**
 * Popup to update/delete Podcast
 */
import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    enter: Transition {
        NumberAnimation { property: "opacity";
            from: 0.0; to: 1.0 ; duration: 300}
    }
    exit: Transition {
        NumberAnimation { property: "opacity";
            from: 1.0; to: 0.0 ; duration: 400}
    }

    GridLayout {
        rows: 4;
        columns: 2;
        rowSpacing: Style.itemSpacings.dense;
        columnSpacing: Style.itemSpacings.dense;

        Text{
            text: podcastControl.title;
            font: Style.font.boldLabel;
            elide: Text.ElideRight;
            color: "white";
            Layout.columnSpan:2;
            Layout.margins:Style.itemMargins.slim;
            Layout.alignment: Qt.AlignCenter | Qt.AlignTop
        }

        IconButton {
            id: refreshBtn
            text: "\uf450"
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            
            Layout.preferredHeight: podcastDeleteBtn.height;
            Layout.preferredWidth: podcastDeleteBtn.width;
            
            onClicked: {
                console.log("refresh "+podcastlist.currentIndex)
                podcastlist.model.refresh(podcastlist.currentIndex);
            }
        }
        Text{
            text: "refresh";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        IconButton {
            id: updateBtn;
            text: "\uf6af";
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            Layout.preferredHeight: podcastDeleteBtn.height;
            Layout.preferredWidth: podcastDeleteBtn.width;

            onClicked: {
                console.log("purge local"+ podcastlist.currentIndex)
                podcastlist.model.purge(podcastlist.currentIndex);
                podcastlist.model.refresh(podcastlist.currentIndex);
            }
        }
        Text{
            text: "purge local";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        DelayButton{
            id: podcastDeleteBtn;
            delay:1000;
            Layout.maximumWidth: 48;
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            contentItem: Text{
                text: "\ufa79"
                color: "white"
                font.pointSize: 20
                horizontalAlignment: Text.AlignHCenter
                font.family: "Material Design Icons"
            }

            onActivated:{
                console.log("Deleting idx: "  + podcastlist.currentIndex)
                podcastlist.model.remove(podcastlist.currentIndex);
                podcastControl.close()
            }
        }
        Text{
            text: "delete";
            font: Style.font.boldLabel;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
    }
}
