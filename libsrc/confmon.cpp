/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 * \since  2017-11-26
 *
 *************************************************************************************/

#include "confmon.hpp"
#include "appconstants.hpp"

using ConfMon = DigitalRooster::ConfMon;

ConfMon::ConfMon() : cfgPath(SYSTEM_CONFIG_PATH) {};


const QString& ConfMon::getCfgPath() {
	return this->cfgPath;
}


void ConfMon::sendSignal(){
//	emit config_changed(this->appCfg);
}
