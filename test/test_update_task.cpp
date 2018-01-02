/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QtDebug>
#include <QUrl>

#include "UpdateTask.hpp"
#include "PodcastSource.hpp"

using namespace std;
using namespace DigitalRooster;



TEST(TestDownload, completed){
	auto ps = make_shared<PodcastSource>(QUrl("https://alternativlos.org/alternativlos.rss"));
	UpdateTask task(ps);
	QSignalSpy spy(&task,  SIGNAL(completed()));
	task.start();
	ASSERT_TRUE(spy.wait());
	ASSERT_EQ(spy.count(), 1);
}

TEST(TestDownload, parsed){
	auto ps = make_shared<PodcastSource>(QUrl("https://alternativlos.org/alternativlos.rss"));
	UpdateTask task(ps);
	QSignalSpy spy(&task,  SIGNAL(completed()));
	task.start();
	ASSERT_TRUE(spy.wait());
	ASSERT_EQ(ps->get_title(), "Alternativlos");
}
