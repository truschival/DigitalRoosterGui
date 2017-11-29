/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include <gtest/gtest.h>
#include "config.hpp"
#include "confmon.hpp"

#include <QSignalSpy>

using namespace DigitalRooster;

TEST(ConfMon,config_file_path)
{
	DigitalRooster::ConfMon cm;
	ASSERT_STREQ(cm.getCfgPath().toStdString().c_str(),DigitalRooster::SYSTEM_CONFIG_PATH.c_str());
}


TEST(ConfMon,sig_conf_changed_emitted){
	DigitalRooster::ConfMon cm;

	QSignalSpy spy(&cm,SIGNAL(config_changed(const AppConfig& )));

	cm.sendSignal();

	ASSERT_EQ( spy.count(), 1);
}
