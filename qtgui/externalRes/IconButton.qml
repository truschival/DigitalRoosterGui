//
// File Copyright (c) 2017 Kevin Carlson
// licensed under SIL Open Font License, version 1.1,
// https://github.com/kevincarlson/QmlBridgeForMaterialDesignIcons

import QtQuick 2.9
import QtQuick.Controls 2.0
import "Icon.js" as MdiFont

Button {
	font.pointSize: 24
	font.family: "Material Design Icons"
	font.weight: Font.DemiBold;
	implicitWidth: Style.buttons.normalH;
	implicitHeight: Style.buttons.normalW;
}
