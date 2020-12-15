// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include "PodcastSource.hpp"
#include "UpdateTask.hpp"
#include <QSignalSpy>
#include <QUrl>
#include <QtDebug>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace std;
using namespace DigitalRooster;

/*****************************************************************************/
TEST(TestDownload, parsed) {
    const QDir cache_dir(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    PodcastSource ps(
        QUrl("https://alternativlos.org/alternativlos.rss"));
    QSignalSpy spy(&ps, SIGNAL(titleChanged()));
    UpdateTask task(&ps);
    ASSERT_TRUE(spy.wait());
    ASSERT_EQ(ps.get_title(), "Alternativlos");
}

/*****************************************************************************/
TEST(TestDownload, donotReEmitEpisodesChanged) {
    const QDir cache_dir(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    PodcastSource ps(
        QUrl("https://alternativlos.org/alternativlos.rss"));
    QSignalSpy spy(&ps, SIGNAL(titleChanged()));
    UpdateTask task(&ps);
    spy.wait(1000);
    QSignalSpy spy2(&ps, SIGNAL(episodesChanged()));
    task.start();
    spy.wait(700);
    ASSERT_EQ(spy.count(), 2);
    ASSERT_EQ(spy2.count(), 0);
    ASSERT_EQ(ps.get_title(), "Alternativlos");
}
