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
#include <QUrl>
#include <QtDebug>
#include <gtest/gtest.h>
#include <stdexcept>
#include "PodcastSource.hpp"
#include "UpdateTask.hpp"

using namespace std;
using namespace DigitalRooster;


TEST(TestDownload, completed) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    UpdateTask task(ps);
    QSignalSpy spy(&task, SIGNAL(newDataAvailable()));
    task.start();
    ASSERT_TRUE(spy.wait());
    ASSERT_EQ(spy.count(), 1);
}

TEST(TestDownload, parsed) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    UpdateTask task(ps);
    QSignalSpy spy(&task, SIGNAL(newDataAvailable()));
    task.start();
    ASSERT_TRUE(spy.wait());
    ASSERT_EQ(ps.get_title(), "Alternativlos");
}

TEST(TestDownload, file_not_readable) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    UpdateTask task(ps);
    EXPECT_THROW(task.newFileAvailable("/dev/mem"),std::system_error);
}


TEST(TestDownload, no_double_parsing_of_same_file) {
    PodcastSource ps(QUrl("https://alternativlos.org/alternativlos.rss"));
    UpdateTask task(ps);
    QSignalSpy spy(&task, SIGNAL(newDataAvailable()));
    task.start();
    spy.wait(700);
    task.start();
    spy.wait(700);
    ASSERT_EQ(spy.count(),1);
    ASSERT_EQ(ps.get_title(), "Alternativlos");
}

