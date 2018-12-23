// Style.qml
import QtQuick 2.0
pragma Singleton

QtObject {
	property int canvasWidth: 320;
	property int canvasHeight: 240;

	property int toolbarWidth: 320;
	property int toolbarHeight: 50;

	property int contentWidth: 320;
	property int contentHeight: canvasHeight-toolbarHeight;


	property QtObject font: QtObject{
		property font clock: Qt.font({
			weight: Font.Bold,
			pointSize: 32
		})

		property font title: Qt.font({
			weight: Font.Bold,
			pointSize: 20
		});

		property font subtitle: Qt.font({
			weight: Font.DemiBold,
			pointSize: 16
		});

		property font listItemHead: Qt.font({
			weight: Font.Bold,
			pointSize: 16
		});

		property font tumbler: Qt.font({
			weight: Font.Bold,
			pointSize: 18
		});

		property font label: Qt.font({
			weight: Font.DemiBold,
			pointSize: 12
		});

		property font valueLabel: Qt.font({
			weight: Font.Normal,
			pointSize: 12
		});

		property font flowText: Qt.font({
			weight: Font.Normal,
			pointSize: 12,
		});

		property font sliderValue: Qt.font({
			weight: Font.DemiBold,
			pointSize: 24
		});
	}// font

	property QtObject buttons: QtObject{
		property int minW: 40;
		property int minH: 40;
		property int normalW: 48;
		property int normalH: 48;
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
