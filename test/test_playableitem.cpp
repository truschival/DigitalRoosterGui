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
    ASSERT_EQ(pi.get_title(),title);
}

/*****************************************************************************/
TEST(PlayableItem, EmitDNChangedPublisher) {
	PlayableItem pi("Foo", QUrl("http://www.heise.de"));

    QSignalSpy spy(&pi, SIGNAL(display_name_changed(const QString&)));
    ASSERT_TRUE(spy.isValid());
    QString publisher("Publisher");
    pi.set_publisher(publisher);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(pi.get_publisher(),publisher);
}

/*****************************************************************************/
TEST(PlayableItem, EmptyTitleEmptyPublisher) {
	PlayableItem pi("Foo", QUrl("http://www.heise.de"));
    ASSERT_EQ(pi.get_display_name(),QString("Foo"));
}

/*****************************************************************************/
TEST(PlayableItem, EmptyTitleButPublisher) {
	PlayableItem pi("Foo", QUrl("http://www.heise.de"));

    QString publisher("Publisher");
    pi.set_publisher(publisher);
    ASSERT_EQ(pi.get_display_name(),publisher);
}

/*****************************************************************************/
TEST(PlayableItem, TitleAndPublisher) {
	PlayableItem pi("Foo", QUrl("http://www.heise.de"));

    QString publisher("Publisher");
    pi.set_publisher(publisher);
    QString title("Cool Title");
    pi.set_title(title);
    ASSERT_EQ(pi.get_display_name(),publisher+": "+title);
}

/*****************************************************************************/
TEST(PlayableItem, ListenedChanged) {
	PodcastEpisode episode("Foo", QUrl("http://www.heise.de"));
	episode.set_duration(1000);
	QSignalSpy spy(&episode,SIGNAL(listened_changed(bool)));
	ASSERT_TRUE(spy.isValid());
	episode.set_position(900);
	spy.wait();
	ASSERT_EQ(spy.count(),1);
    ASSERT_TRUE(episode.already_listened());
}


