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

using ConfMon = DigitalRooster::ConfMon;

const QString& ConfMon::getCfgPath() {
	return this->cfgPath;
}


void ConfMon::sendSignal(){
	emit config_changed(this->appCfg);
}
