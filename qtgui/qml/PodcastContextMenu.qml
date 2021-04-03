// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.4

Menu {
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    modal:true;
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    Timer {
        id: podcastCtxMenuCloseTimer;
        interval: applicationWindow.dialogTimeout;
        running: false;
        repeat: false;
        onTriggered: parent.close();
    }

    onAboutToShow : {
        podcastCtxMenuCloseTimer.start();
    }

    GridLayout {
        rows: 4;
        columns: 2;
        rowSpacing: Style.itemSpacings.dense;
        columnSpacing: Style.itemSpacings.dense;

        Text{
            text: podcastControl.title;
            font: Style.font.label;
            elide: Text.ElideMiddle;
            color: Material.accent;

            Layout.columnSpan:2;
            Layout.margins:Style.itemMargins.slim;
            Layout.maximumWidth: parent.width;
            Layout.alignment: Qt.AlignCenter | Qt.AlignTop
        }

        IconButton {
            id: refreshBtn
            text: "\uf450";
            Layout.alignment:  Qt.AlignCenter | Qt.AlignVCenter

            Layout.preferredHeight: podcastDeleteBtn.height;
            Layout.preferredWidth: podcastDeleteBtn.width;

            onClicked: {
                podcastlist.model.refresh(podcastlist.currentIndex);
                podcastControl.close()
            }
        }

        Text{
            text: "refresh";
            font: Style.font.label;
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
                podcastlist.model.purge(podcastlist.currentIndex);
                podcastlist.model.refresh(podcastlist.currentIndex);
                podcastControl.close()
            }
        }

        Text{
            text: "purge local";
            font: Style.font.label;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        DelayButton{
            id: podcastDeleteBtn;
            delay:1000;
            Layout.maximumWidth: 48;
            Layout.alignment:  Qt.AlignCenter | Qt.AlignVCenter
            contentItem: Text{
                text: "\ufa79"
                color: "white"
                font.pointSize: 20
                horizontalAlignment: Text.AlignHCenter
                font.family: "Material Design Icons"
            }

            onActivated:{
                podcastlist.model.remove(podcastlist.currentIndex);
                podcastControl.close()
            }
        }

        Text{
            text: "delete";
            font: Style.font.label;
            color: "white"
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }
    }
}
