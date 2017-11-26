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

TEST(ConfMon,config_file_path)
{
	DigitalRooster::ConfMon cm;
	ASSERT_STREQ(cm.getCfgPath().toStdString().c_str(),DigitalRooster::SYSTEM_CONFIG_PATH.c_str());
}

