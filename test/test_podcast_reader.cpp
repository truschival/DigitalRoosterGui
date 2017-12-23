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

#include "PodcastSourceReader.hpp"

using namespace DigitalRooster;

TEST(DownloadRSS,downloadOk){
	PodcastSourceReader sr;
	ASSERT_STREQ(sr.download_rss("").toStdString().c_str(),"alternativlos.rss");
}
