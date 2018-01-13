/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include "PodcastSourceReader.hpp"
#include <QMap>
#include <config.h>
#include <gtest/gtest.h>

using namespace DigitalRooster;

TEST(DownloadRSS, downloadOk) {
    PodcastSourceReader sr;
    ASSERT_STREQ(sr.download_rss("").toStdString().c_str(), "alternativlos.rss");
}

TEST(PodcastSourceReader, parseInfo_good) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos.rss");
    sr.update_podcast(ps);

    EXPECT_EQ(ps.get_title(), QString("Alternativlos"));
    EXPECT_EQ(ps.get_link().toString(), QString("http://www.alternativlos.org/"));
    EXPECT_STREQ(ps.get_description().toStdString().substr(0, 58).c_str(),
        "Alternativlos ist der Boulevard-Podcast von Frank und Fefe");
}

TEST(PodcastSourceReader, parseInfo_good_element_count) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos.rss");
    sr.update_podcast(ps);
    const auto& episodes = ps.get_episodes();
    EXPECT_EQ(episodes.size(), 27);
}

TEST(PodcastSourceReader, parseInfo_bad_malformatted) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos_malformatted.rss");
    /* all exceptions should be caught */
    EXPECT_NO_THROW(sr.update_podcast(ps));
}

TEST(PodcastSourceReader, parseInfo_bad_missing_title) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos_bad.rss");
    sr.update_podcast(ps);
    const auto& episodes = ps.get_episodes();
    EXPECT_EQ(episodes.size(), 25);
}

TEST(PodcastSourceReader, parseInfo_bad_missing_url) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos_bad.rss");
    sr.update_podcast(ps);
    const auto& episodes = ps.get_episodes();
    EXPECT_EQ(episodes.size(), 25);
}


TEST(PodcastSourceReader, maxEpisodesReached) {
    const size_t maxepisodes = 14;
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps(
        "https://alternativlos.org/alternativlos.rss"); // has many episodes
    ps.set_rss_file("./alternativlos.rss");
    ps.set_max_episodes(maxepisodes);
    sr.update_podcast(ps);

    const auto& episodes = ps.get_episodes();
    ASSERT_GT(episodes.size(), 1);
    ASSERT_LE(episodes.size(), maxepisodes);
}

TEST(PodcastSourceReader, getEpisodeNames) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos.rss");
    sr.update_podcast(ps);

    const auto& episodes = ps.get_episodes_names();
    ASSERT_GT(episodes.size(), 1);
    EXPECT_STREQ(episodes[1].toStdString().substr(0, 6).c_str(), "ALT016");
}

TEST(PodcastSourceReader, getEpisodes) {
    PodcastSourceReader sr;
    PodcastSourceConfiguration ps("https://alternativlos.org/alternativlos.rss");
    ps.set_rss_file("./alternativlos.rss");
    sr.update_podcast(ps);

    const auto& episodes = ps.get_episodes();
    ASSERT_GT(episodes.size(), 1);
    EXPECT_STREQ(
        episodes[1]->get_display_name().toStdString().substr(0, 6).c_str(), "ALT016");
    EXPECT_STREQ(episodes[1]->get_url().toString().toStdString().c_str(),
        "http://alternativlos.cdn.as250.net/alternativlos-16.mp3");
}
