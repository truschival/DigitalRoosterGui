// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

/* Parser tests do not need wpa_supplicant and are always built */

#include <cstring>
#include <stdexcept>
#include <QString>

#include "gtest/gtest.h"
#include "wifi_control.hpp"

using namespace DigitalRooster;

/* clang-format off */
const char* scan_result =
		"bssid / frequency / signal level / flags / ssid\n"
		"34:31:c4:e1:d3:97	2462	-60	[WPA2-PSK-CCMP][ESS]	Canto do Chimarrao\n"
		"cc:ce:1e:ca:d3:72	2462	-62	[WPA2-PSK-CCMP][WPS][ESS]	FRITZ!Box 7430 LF\n"
		"0c:80:63:e1:78:0c	2422	-83	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]	WeirdNet\n"
		"5c:dc:96:c1:dc:b4	2462	-78	[WPA2-PSK-CCMP][WPS][ESS]	WLAN-174607\n"
		"58:d7:59:8f:19:10	2412	-83	[WPA2-PSK-CCMP][WPS][ESS]	WLAN-PXSRVK\n"
		"dc:53:7c:cb:2a:1b	2412	-83	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][WPS][ESS]	UPC23C42CD\n"
		"de:53:1c:cb:2a:1b	2412	-83	[WPA2-EAP-CCMP+TKIP][ESS]	Unitymedia WifiSpot\n"
		"a8:f5:ac:60:f0:60	2437	-83	[WPA2-PSK-CCMP][WPS][ESS]	WLAN-TE77VQ\n"
		"54:67:51:c9:5c:b1	2462	-83	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][WPS][ESS]	UPC2F79B17\n"
		"56:67:11:c9:5c:b1	2462	-83	[WPA2-EAP-CCMP+TKIP][ESS]	Unitymedia WifiSpot\n"
		"5c:dc:96:c1:dc:b5	2462	-78	[ESS]	Telekom_FON\n"
		"c8:0c:c8:29:f7:c6	2462	-83	[ESS]	Telekom_FON\n";
/* clang-format on */
/*****************************************************************************/

TEST(WifiControl, parseBuffer){
	auto networks = parse_scanresult(scan_result, strlen(scan_result));
	ASSERT_EQ(networks.size(),12);
	ASSERT_EQ(networks[0].name, QString("Canto do Chimarrao"));
	ASSERT_EQ(networks[0].signal_strength, -60);
	ASSERT_EQ(networks[0].bssid, QString("34:31:c4:e1:d3:97"));
}

/*****************************************************************************/
TEST(WifiControl, parseOnlyGivenLength) {
    // reply buffer still contains data of a previous, longer reply
    std::string buffer(scan_result);
    auto first_two_networks = buffer.find("0c:80:63");
    auto networks = parse_scanresult(buffer.c_str(), first_two_networks);
    ASSERT_EQ(networks.size(), 2);
}

/*****************************************************************************/
TEST(WifiControl, lineWithMissingSsidThrows) {
    QString line("34:31:c4:e1:d3:97\t2462\t-60\t[ESS]");
    ASSERT_THROW(line_to_network(QStringRef(&line)), std::runtime_error);
}
