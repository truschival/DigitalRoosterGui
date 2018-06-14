/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#include <QMap>
#include <gtest/gtest.h>
#include <stdexcept> // std::system_error

#include "appconstants.hpp"
#include "PodcastSourceReader.hpp"

using namespace DigitalRooster;

TEST(PodcastSourceReader, parseInfo_good) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);

	EXPECT_EQ(ps.get_title(), QString("Alternativlos"));
	EXPECT_EQ(ps.get_link().toString(),
			QString("http://www.alternativlos.org/"));
	EXPECT_STREQ(ps.get_description().toStdString().substr(0, 58).c_str(),
			"Alternativlos ist der Boulevard-Podcast von Frank und Fefe");
}

TEST(PodcastSourceReader, parseInfo_good_element_count) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto& episodes = ps.get_episodes();
	EXPECT_EQ(episodes.size(), 27);
}

TEST(PodcastSourceReader, parseInfo_episode_length) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto episodes = ps.get_episodes();
	auto title = episodes[20];
	// ALT21 duration: 1:35:39 =(1*60*60+35*60+39)*1000 =5739000
	EXPECT_EQ(title->get_duration(), 5739000);
}

TEST(PodcastSourceReader, parseInfo_episode_length_mm_ss) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto episodes = ps.get_episodes();
	auto title = episodes[21];
	// ALT20 duration: 12:02 = 12*60+02)*1000 =722000
	EXPECT_EQ(title->get_duration(), 722000);
}

TEST(PodcastSourceReader, parseInfo_episode_length_m_ss) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto episodes = ps.get_episodes();
	auto title = episodes[22];
	// ALT019 duration: 9:57 = (9*60+57)*1000 =597000
	EXPECT_EQ(title->get_duration(), 597000);
}

TEST(PodcastSourceReader, parseInfo_episode_length_ss) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto episodes = ps.get_episodes();
	auto title = episodes[23];
	// ALT018 duration: 57 = (57)*1000 =57
	EXPECT_EQ(title->get_duration(), 57000);
}

TEST(PodcastSourceReader, parseInfo_episode_display_name) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto episodes = ps.get_episodes();
	auto title = episodes[19];
	EXPECT_EQ(title->get_display_name(), QString("ALT022: Korruption"));
}

TEST(PodcastSourceReader, parseInfo_episode_pubdate) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);
	const auto episodes = ps.get_episodes();
	auto title = episodes[19]; // ALT22
	auto testtime = QDateTime::fromString("Mon, 05 Mar 2012 22:00:00 GMT",
			Qt::DateFormat::RFC2822Date);
	EXPECT_EQ(title->get_publication_date().toTime_t(), testtime.toTime_t());
}

TEST(PodcastSourceReader, parseInfo_episode_url) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos.rss");
	update_podcast(ps);

	const auto episodes = ps.get_episodes();
	EXPECT_STREQ(episodes[1]->get_url().toString().toStdString().c_str(),
			"http://alternativlos.cdn.as250.net/alternativlos-40.mp3");
}

TEST(PodcastSourceReader, parseInfo_bad_nonexistent) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/doesnotexist.rss");
	EXPECT_THROW(update_podcast(ps), std::system_error);
}

TEST(PodcastSourceReader, parseInfo_bad_malformatted) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos_malformatted.rss");
	/* all exceptions should be caught */
	EXPECT_NO_THROW(update_podcast(ps));
}

TEST(PodcastSourceReader, parseInfo_bad_missing_title) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos_bad.rss");
	update_podcast(ps);
	const auto& episodes = ps.get_episodes();
	EXPECT_EQ(episodes.size(), 25);
}

TEST(PodcastSourceReader, parseInfo_bad_missing_url) {
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
	ps.set_rss_file(TEST_FILE_PATH + "/alternativlos_bad.rss");
	update_podcast(ps);
	const auto& episodes = ps.get_episodes();
	EXPECT_EQ(episodes.size(), 25);
}

TEST(PodcastSourceReader, maxEpisodesReached) {
	const int maxepisodes = 14;
	PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss")); // has many episodes
	ps.set_rss_file(TEST_FILE_PATH+"/alternativlos.rss");
	ps.set_max_episodes(maxepisodes);
	update_podcast(ps);

	const auto& episodes = ps.get_episodes();
	ASSERT_GT(episodes.size(), 1);
	ASSERT_LE(episodes.size(), maxepisodes);
}
