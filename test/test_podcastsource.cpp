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
TEST(PodcastSource, dont_add_twice) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    auto pi =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi);
    ps.add_episode(pi);
    EXPECT_EQ(ps.get_episodes().size(), 1);
}

/******************************************************************************/
TEST(PodcastSource, add_two_with_guid) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
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
TEST(PodcastSource, add_episodeEmitsCountChanged) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
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
TEST(PodcastSource, get_episode_names) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    auto pi =
        std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi);
    EXPECT_EQ(ps.get_episodes_names().size(), 1);
    auto ep = ps.get_episodes_names()[0];
    EXPECT_EQ(ep, QString("TheName"));
}

/******************************************************************************/
TEST(PodcastSource, getEpisodeById) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
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
TEST(PodcastSource, set_updater) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    ps.set_update_task(std::make_unique<UpdateTask>());
    ps.set_update_interval(std::chrono::seconds(1));

    QSignalSpy spy(&ps, SIGNAL(dataChanged()));
    spy.wait(7000);
    ASSERT_GE(spy.count(), 1);
}

/******************************************************************************/
TEST(PodcastSource, getFileName) {
    QUuid uid = QUuid::createUuid();
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"), uid);
    auto filename = ps.get_cache_file_name();
    QString expected_filename(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    expected_filename += QDir::separator() + uid.toString();

    ASSERT_EQ(filename, expected_filename);
}

/******************************************************************************/
