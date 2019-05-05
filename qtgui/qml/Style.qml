// Style.qml
import QtQuick 2.0
pragma Singleton

QtObject {
	property int canvasWidth: 320;
	property int canvasHeight: 240;

	property int toolbarWidth: 320;
    property int toolbarHeight: 42;

	property int contentWidth: 320;
	property int contentHeight: canvasHeight-toolbarHeight;


	property QtObject colors: QtObject{
		property  color selected :  "#2196F3";
		property color unselected: "lightgray";
		property color disabled: "gainsboro";
		property color enabled: "#2196F3";
	}

	property QtObject font: QtObject{

		property font title: Qt.font({
            weight: Font.Normal,
			pointSize: 16
		});
		
		property font titleBold: Qt.font({
            weight: Font.Bold,
            pointSize: 16
        });

		property font subtitle: Qt.font({
			family: "DejaVu Sans Condensed",
			weight: Font.DemiBold,
			pointSize: 12
		});

		property font listItemHead: Qt.font({
			family: "DejaVu Sans Condensed",
            weight: Font.DemiBold,
			letterSpacing:-1,
			pointSize: 12
		});

		property font listItemHeadListened: Qt.font({
			family: "DejaVu Sans Condensed",
			weight: Font.Normal,
			letterSpacing:-1,
			pointSize: 12
		});

		property font label: Qt.font({
			family: "DejaVu Sans Condensed",
			weight: Font.DemiBold,
			letterSpacing:-1,
			pointSize: 10
		});

		property font boldLabel: Qt.font({
			family: "DejaVu Sans Condensed",
			weight: Font.Bold,
			letterSpacing:-1,
			pointSize: 10
		});

		property font valueLabel: Qt.font({
			family: "DejaVu Sans Condensed",
			weight: Font.Normal,
			letterSpacing:-1,
			pointSize: 10
		});

		property font flowText: Qt.font({
			family: "DejaVu Sans Condensed",
			weight: Font.Normal,
			letterSpacing:-1,
			pointSize: 10,
		});

		// -- elements
		property font clock: Qt.font({
			weight: Font.Bold,
			pointSize: 42
		})

		property font tumbler: Qt.font({
			weight: Font.Bold,
            pointSize: 16
		});

		property font button: Qt.font({
			family: "Material Design Icons",
			weight: Font.DemiBold,
			pointSize: 18
		});

		property font sliderValue: Qt.font({
			weight: Font.DemiBold,
			pointSize: 16
		});
	}// font

	property QtObject buttons: QtObject{
		property int minW: 42;
		property int minH: 42;
		property int normalW: 44;
		property int normalH: 44;
	}

	property QtObject drawer: QtObject{
		property int w: 48;
		property int h: 48;
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
