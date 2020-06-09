import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

ListView {
    id:iradiolist
    property string objectName : "InternetRadio"
    width: stackView.width
    height: stackView.height
    contentWidth: stackView.width

    rebound: listBoundTransition;

    delegate: IRadioDelegate{
        id: iradiodelegate
    }

    model: iradiolistmodel

    Connections {
        onFlickStarted : {
            viewResetTimer.restart();
        }
    }
}
