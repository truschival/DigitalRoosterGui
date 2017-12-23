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

TEST(PodcastSourceReader,parseInfo){
	PodcastSourceReader sr;
	PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
	ps.set_rss_feed("./alternativlos.rss");
	sr.update_podcast(ps);

	ASSERT_STREQ(ps.get_title().toStdString().c_str(),"Alternativlos");
}
