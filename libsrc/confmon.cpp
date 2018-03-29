/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#include "confmon.hpp"
#include "appconstants.hpp"

using ConfMon = DigitalRooster::ConfMon;

ConfMon::ConfMon() : cfgPath(SYSTEM_CONFIG_PATH) {};


const QString& ConfMon::getCfgPath() {
	return this->cfgPath;
}


void ConfMon::sendSignal(){
	emit config_changed();
}
