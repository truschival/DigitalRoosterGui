/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/


#include <config.h>
#include <gtest/gtest.h>
#include "mediaplayer_wrapper.hpp"


using namespace std;
using namespace DigitalRooster;

TEST(TestPlayer, no_media){
	MediaPlayerWrapper mp;

	auto url = QUrl::fromLocalFile("/usr/share/sounds/Oxygen-Im-Connection-Lost.ogg");
	mp.play(url);

}
