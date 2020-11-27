// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <gtest/gtest.h>

#include "PlayableItem.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using namespace std;

/*****************************************************************************/

TEST(PlayableItem, EmitDNChangedTitle) {
    PlayableItem pi("Foo", QUrl("http://www.heise.de"));
    QSignalSpy spy(&pi, SIGNAL(display_name_changed(const QString&)));
    ASSERT_TRUE(spy.isValid());
    QString title("Cool Title");
    pi.set_title(title);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(pi.get_title(), title);
}

/*****************************************************************************/
TEST(PlayableItem, EmitDNChangedPublisher) {
    PlayableItem pi("Foo", QUrl("http://www.heise.de"));

    QSignalSpy spy(&pi, SIGNAL(display_name_changed(const QString&)));
    ASSERT_TRUE(spy.isValid());
    QString publisher("Publisher");
    pi.set_publisher(publisher);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(pi.get_publisher(), publisher);
}

/*****************************************************************************/
TEST(PlayableItem, DefaultGetDisplayName) {
    PlayableItem pi("Foo", QUrl("http://www.heise.de"));
    ASSERT_EQ(pi.get_display_name(), QString("Foo"));
}

/*****************************************************************************/
TEST(PodcastEpisode, EmptyTitleEmptyPublisher) {
    PodcastEpisode pi("Foo", QUrl("http://www.heise.de"));
    ASSERT_EQ(pi.get_display_name(), QString("Foo"));
}

/*****************************************************************************/
TEST(PodcastEpisode, EmptyTitleButPublisher) {
    PodcastEpisode pi("Foo", QUrl("http://www.heise.de"));

    QString publisher("Publisher");
    pi.set_publisher(publisher);
    ASSERT_EQ(pi.get_display_name(), QString("Publisher: Foo"));
}

/*****************************************************************************/
TEST(PodcastEpisode, TitleAndPublisher) {
    PodcastEpisode pi("Foo", QUrl("http://www.heise.de"));

    QString publisher("Publisher");
    pi.set_publisher(publisher);
    QString title("Cool Title");
    pi.set_title(title);

    ASSERT_EQ(pi.get_display_name(), QString("Publisher: Cool Title"));
}

/*****************************************************************************/
TEST(PlayableItem, ListenedChanged) {
    PodcastEpisode episode("Foo", QUrl("http://www.heise.de"));
    episode.set_duration(1000);
    QSignalSpy spy(&episode, SIGNAL(listened_changed(bool)));
    ASSERT_TRUE(spy.isValid());
    episode.set_position(900);
    spy.wait();
    ASSERT_EQ(spy.count(), 1);
    ASSERT_TRUE(episode.already_listened());
}

/*****************************************************************************/
TEST(PlayableItem, from_json_ok) {
    QString json_string(R"(
        {
            "id": "{de2c79da-c250-4c78-a2db-5db398c0cbd9}",
            "name": "Radio FM4",
            "url": "https://fm4shoutcast.sf.apa.at"
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    auto ps = PlayableItem::from_json_object(jdoc.object());
    ASSERT_EQ(ps->get_display_name(), QString("Radio FM4"));
    ASSERT_EQ(ps->get_url(), QUrl("https://fm4shoutcast.sf.apa.at"));
    ASSERT_EQ(ps->get_id(),
        QUuid::fromString(QString("{de2c79da-c250-4c78-a2db-5db398c0cbd9}")));
}

/*****************************************************************************/
TEST(PlayableItem, from_json_invalid) {
    QString json_string(R"(
        {
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    ASSERT_THROW(
        PlayableItem::from_json_object(jdoc.object()), std::invalid_argument);
}

/*****************************************************************************/
TEST(PlayableItem, from_json_invalid_uuid) {
    QString json_string(R"(
        {
	        "id": "123-432-ABCD",
            "name": "the station name",
            "url": "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3"
        }
	)");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    ASSERT_THROW(
        PlayableItem::from_json_object(jdoc.object()), std::invalid_argument);
}

/*****************************************************************************/
