/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include <QSignalSpy>
#include <QUrl>
#include <QtDebug>
#include <gtest/gtest.h>

#include "PodcastSource.hpp"
#include "UpdateTask.hpp"

using namespace std;
using namespace DigitalRooster;


TEST(TestDownload, completed) {
    auto ps =
        make_shared<PodcastSource>(QUrl("https://alternativlos.org/alternativlos.rss"));
    UpdateTask task(ps);
    QSignalSpy spy(&task, SIGNAL(completed()));
    task.start();
    ASSERT_TRUE(spy.wait());
    ASSERT_EQ(spy.count(), 1);
}

TEST(TestDownload, parsed) {
    auto ps =
        make_shared<PodcastSource>(QUrl("https://alternativlos.org/alternativlos.rss"));
    UpdateTask task(ps);
    QSignalSpy spy(&task, SIGNAL(completed()));
    task.start();
    ASSERT_TRUE(spy.wait());
    ASSERT_EQ(ps->get_title(), "Alternativlos");
}
