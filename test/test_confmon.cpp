/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include "confmon.hpp"
#include <config.h>
#include <gtest/gtest.h>

#include <QSignalSpy>

using namespace DigitalRooster;

TEST(ConfMon, config_file_path) {
    DigitalRooster::ConfMon cm;
    ASSERT_EQ(cm.getCfgPath(), DigitalRooster::SYSTEM_CONFIG_PATH);
}

TEST(ConfMon, sig_conf_changed_emitted) {
    DigitalRooster::ConfMon cm;

    QSignalSpy spy(&cm, SIGNAL(config_changed(const AppConfig&)));

    cm.sendSignal();

    ASSERT_EQ(spy.count(), 1);
}
