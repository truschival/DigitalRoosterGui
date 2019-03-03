import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

ListView {
    id: podcastlist
    property string objectName : "PodcastList"
    width: stackView.width
    height: stackView.height
    antialiasing: true
    highlightRangeMode: ListView.ApplyRange
    boundsBehavior: Flickable.StopAtBounds
    maximumFlickVelocity: 1500
    flickDeceleration: 1800
    snapMode: ListView.SnapOneItem
    contentWidth: stackView.width
    focus: true

    delegate: PodcastDelegate{
        id: podcastdelegate
    }
    model: podcastmodel

    /**
     * Popup to update/delete Podcast
     */
    Menu {
	id: podcastControl

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

	RowLayout {
	    spacing: Style.itemSpacings.medium;
	    
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

	    IconButton {
		id: updateBtn; 
		text: "\uf6af";
		Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
		Layout.preferredHeight: podcastDeleteBtn.height;
		Layout.preferredWidth: podcastDeleteBtn.width;

		onClicked: {
		    console.log("purge & update"+ podcastlist.currentIndex)
		    podcastlist.model.purge(podcastlist.currentIndex);
    		    podcastlist.model.refresh(podcastlist.currentIndex);
		}
	    }

	    DelayButton{
		id: podcastDeleteBtn;
		delay:1000;
		
		contentItem: Text{
		    text: "\ufa79"
		    color: "white"
		    font.pointSize: 24
		    horizontalAlignment: Text.AlignHCenter
    		    font.family: "Material Design Icons"
		}

		onActivated:{
		    console.log("Deleting idx: "  + podcastlist.currentIndex) 
		    podcastlist.model.remove(podcastlist.currentIndex);
		    podcastControl.close()
		}
	    }
	}
    }
}
