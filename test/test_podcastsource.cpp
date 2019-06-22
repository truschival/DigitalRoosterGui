/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <appconstants.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept> // std::system_error

#include <QSignalSpy>
#include <QString>
#include <QUrl>

#include "PlayableItem.hpp"
#include "PodcastSource.hpp"

using namespace DigitalRooster;

/******************************************************************************/

class PodcastSourceFixture : public virtual ::testing::Test {
public:
    PodcastSourceFixture()
        : cache_dir(
              TEST_FILE_PATH + QDir::separator() + QString("podcast_cache"))
        , uid(QUuid::createUuid())
        , ps(QUrl("https://alternativlos.org/alternativlos.rss"), cache_dir,
              uid) {
    }

    ~PodcastSourceFixture() {
    }

    void SetUp() {
        cache_dir.mkpath(".");
    }

    void TearDown() {
        cache_dir.removeRecursively();
    }

protected:
    QDir cache_dir;
    QUuid uid;
    PodcastSource ps;
};

/******************************************************************************/
TEST_F(PodcastSourceFixture, dont_add_twice) {
    auto pi =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi);
    ps.add_episode(pi);
    EXPECT_EQ(ps.get_episodes().size(), 1);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, add_two_with_guid) {
    auto pi1 =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    pi1->set_guid("FooBAR");
    ps.add_episode(pi1);
    auto pi2 =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi2);
    EXPECT_EQ(ps.get_episodes().size(), 2);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, add_episodeEmitsCountChanged) {
    auto pi1 =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    QSignalSpy spy(&ps, SIGNAL(episodes_count_changed(int)));
    ps.add_episode(pi1);

    spy.wait(300);
    ASSERT_EQ(spy.count(), 1);
    // arguments of first signal, here only 1 int
    // QSignalSpy inherits from QList<QList<QVariant>>
    QList<QVariant> arguments = spy.takeFirst();
    ASSERT_EQ(arguments.at(0).toInt(), 1);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, get_episode_names) {
    auto pi =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi);
    EXPECT_EQ(ps.get_episodes_names().size(), 1);
    auto ep = ps.get_episodes_names()[0];
    EXPECT_EQ(ep, QString("TheName"));
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, getEpisodeById) {
    auto first =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(first);
    auto second_with_guid = std::make_shared<PodcastEpisode>(
        "The other name", QUrl("http://other_url.bar"));
    second_with_guid->set_guid("MyGUID");
    ps.add_episode(second_with_guid);
    EXPECT_EQ(ps.get_episodes_names().size(), 2);

    auto ep1 = ps.get_episode_by_id("http://foo.bar");
    EXPECT_EQ(ep1, first);

    auto ep2 = ps.get_episode_by_id("MyGUID");
    EXPECT_EQ(ep2, second_with_guid);

    auto non_existent = ps.get_episode_by_id("weird_id");
    EXPECT_FALSE(non_existent);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, set_updater) {
    ps.set_update_task(std::make_unique<UpdateTask>());
    ps.set_update_interval(std::chrono::seconds(1));

    QSignalSpy spy(&ps, SIGNAL(dataChanged()));
    spy.wait(7000);
    ASSERT_GE(spy.count(), 1);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, getFileName) {
    auto filename = ps.get_cache_file_name();
    QString expected_filename(cache_dir.filePath(uid.toString()));

    ASSERT_EQ(filename, expected_filename);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, storeAndPurgeworks) {
    auto filename = ps.get_cache_file_name();
    QFile cachefile(filename);
    ASSERT_FALSE(cachefile.exists());
    ps.set_description("MyDescription");
    auto first =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(first);
    ps.store();
    ASSERT_TRUE(cachefile.exists());
    ps.purge();
    ASSERT_FALSE(cachefile.exists());
    ASSERT_EQ(ps.get_episode_count(),0);
}

/******************************************************************************/
TEST(PodcastSource, store_bad_nothrow) {
	QDir cache_dir_bad("/some/nonexistent/cache/dir");
    PodcastSource ps(QUrl("http://foo.bar"),cache_dir_bad);
    ps.set_description("MyDescription");
    auto first =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(first);
    ASSERT_NO_THROW(ps.store());
}

/******************************************************************************/


