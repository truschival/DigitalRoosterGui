// Style.qml
import QtQuick 2.0
pragma Singleton

QtObject {
	property int canvasWidth: 320;
	property int canvasHeight: 240;

	property int toolbarWidth: 320;
	property int toolbarHeight: 64;

	property int contentWidth: 320;
	property int contentHeight: canvasHeight-toolbarHeight;
		
	property font h1Font: Qt.font({
        weight: Font.DemiBold,
        pointSize: 20
    })

	property font h2Font: Qt.font({
        weight: Font.Normal,
        pointSize: 16
    });
	
	property font h3Font: Qt.font({
        weight: Font.Normal,
        pointSize: 16
    });
	
	property font h4Font: Qt.font({
        weight: Font.Normal,
        pointSize: 14
    });

	property font labelFont: Qt.font({
        weight: Font.Normal,
        pointSize: 12
    });

	property font importantLabelFont: Qt.font({
        weight: Font.DemiBold,
        pointSize: 12
    });
	
	property QtObject buttons: QtObject{
		property int w: 48;
		property int h: 48;
		property int pw: 58;
		property int ph: 58;		
	}

	property QtObject drawer: QtObject{
		property int w: 64;
		property int h: 64;
		property int dragmargin: 20;
	}

	property QtObject itemSpacings: QtObject{
		property int dense: 2;
		property int medium: 4;
		property int sparse: 6;
	}

	property QtObject itemMargins: QtObject{
		property int slim: 2;
		property int medium: 4;
		property int wide: 6;
		property int extrawide: 10;		
	}	
}
