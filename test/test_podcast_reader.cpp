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
#include <QMap>
#include "PodcastSourceReader.hpp"

using namespace DigitalRooster;

TEST(DownloadRSS,downloadOk) {
	PodcastSourceReader sr;
	ASSERT_STREQ(sr.download_rss("").toStdString().c_str(),
			"alternativlos.rss");
}

TEST(PodcastSourceReader,parseInfo_good) {
	PodcastSourceReader sr;
	PodcastSourceConfiguration ps(
			"https://alternativlos.org/alternativlos.rss");
	ps.set_rss_file("./alternativlos.rss");
	sr.update_podcast(ps);

	EXPECT_STREQ(ps.get_title().toStdString().c_str(), "Alternativlos");
	EXPECT_STREQ(ps.get_link().toStdString().c_str(),
			"http://www.alternativlos.org/");
	EXPECT_STREQ(ps.get_description().toStdString().substr(0, 58).c_str(),
			"Alternativlos ist der Boulevard-Podcast von Frank und Fefe");
}

TEST(PodcastSourceReader,parseInfo_bad) {
	PodcastSourceReader sr;
	PodcastSourceConfiguration ps(
			"https://alternativlos.org/alternativlos.rss");
	ps.set_rss_file("./alternativlos_bad.rss");
	/* all exceptions should be caught */
	EXPECT_NO_THROW(sr.update_podcast(ps));
}


TEST(PodcastSourceReader,maxEpisodesReached) {
	const  size_t maxepisodes = 2;
	PodcastSourceReader sr;
	PodcastSourceConfiguration ps(
			"https://alternativlos.org/alternativlos.rss"); // has 3 episodes
	ps.set_rss_file("./alternativlos.rss");
	ps.set_max_episodes(maxepisodes);
	sr.update_podcast(ps);

	const auto& episodes = ps.get_episodes();
	ASSERT_GT(episodes.size(), 1);
	ASSERT_LE(episodes.size(), maxepisodes);

}

TEST(PodcastSourceReader,getEpisodeNames) {
	PodcastSourceReader sr;
	PodcastSourceConfiguration ps(
			"https://alternativlos.org/alternativlos.rss");
	ps.set_rss_file("./alternativlos.rss");
	sr.update_podcast(ps);

	const auto& episodes = ps.get_episodes_names();
	ASSERT_GT(episodes.size(), 1);
	EXPECT_STREQ(episodes[1].toStdString().substr(0, 6).c_str(), "ALT040");
}

TEST(PodcastSourceReader,getEpisodes) {
	PodcastSourceReader sr;
	PodcastSourceConfiguration ps(
			"https://alternativlos.org/alternativlos.rss");
	ps.set_rss_file("./alternativlos.rss");
	sr.update_podcast(ps);

	const auto& episodes = ps.get_episodes();
	ASSERT_GT(episodes.size(), 1);
	EXPECT_STREQ(
			episodes[1]->get_display_name().toStdString().substr(0, 6).c_str(),
			"ALT040");
	EXPECT_STREQ(episodes[1]->get_url().toString().toStdString().c_str(),
			"http://alternativlos.cdn.as250.net/alternativlos-40.mp3");
}

