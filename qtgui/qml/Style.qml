// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2021  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */
import QtQuick 2.11
import QtQuick.Controls.Material 2.4

pragma Singleton

QtObject {
    property int canvasWidth: 320;
    property int canvasHeight: 240;

    property int toolbarWidth: 320;
    property int toolbarHeight: 42;

    property int contentWidth: 320;
    property int contentHeight: canvasHeight-toolbarHeight;

    property QtObject colors: QtObject{
        property color primaryText : "white";
        property color widgetBackground : Material.background;
        property color selected :   Material.accent; // "#2196F3";
        property color unselected:  Material.color(Material.Grey) ;// "gainsboro";
        property color disabled: Material.color(Material.Grey);
        property color enabled: Material.primary; //"#2196F3";
    }

    function scaleFont(px){
        return Math.ceil(px*1.5);
    }

    property QtObject font: QtObject{

        property font title: Qt.font({
            weight: Font.Normal,
            pixelSize: scaleFont(18),
            preferShaping: false
        });

        property font titleBold: Qt.font({
            weight: Font.Bold,
            pixelSize: scaleFont(18),
            preferShaping: false
        });

        property font subtitle: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.DemiBold,
            pixelSize: scaleFont(14),
            preferShaping: false
        });

        property font weatherInfo: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.Bold,
            pixelSize: scaleFont(14),
            preferShaping: false
        });

        property font weatherTime: Qt.font({
            family: "DejaVu Sans",
            weight: Font.DemiBold,
            pixelSize: scaleFont(14),
            preferShaping: false
        });

        property font listItemHead: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.DemiBold,
            pixelSize: scaleFont(14),
            preferShaping: false
        });

        property font listItemHeadListened: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.Normal,
            pixelSize: scaleFont(14),
            preferShaping: false
        });

        property font label: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.DemiBold,
            letterSpacing:-1,
            pixelSize: scaleFont(12),
            preferShaping: false
        });

        property font valueLabel: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.Normal,
            letterSpacing:-1,
            pixelSize: scaleFont(12),
            preferShaping: false
        });

        property font flowText: Qt.font({
            family: "DejaVu Sans Condensed",
            weight: Font.Normal,
            letterSpacing:-1,
            pixelSize: scaleFont(10),
            preferShaping: false
        });

        // -- elements
        property font clock: Qt.font({
            weight: Font.Bold,
            pixelSize: scaleFont(64),
            preferShaping: false
        })

        property font tumbler: Qt.font({
            weight: Font.Bold,
            pixelSize: scaleFont(16),
            preferShaping: false
        });

        property font button: Qt.font({
            family: "Material Design Icons",
            weight: Font.DemiBold,
            pixelSize: scaleFont(20)
        });

        property font sliderValue: Qt.font({
            weight: Font.DemiBold,
            pixelSize: scaleFont(16),
            preferShaping: false
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
