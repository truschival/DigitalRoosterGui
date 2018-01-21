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
#include <stdexcept> // std::system_error
#include <memory>

#include <QString>
#include <QUrl>
#include <QSignalSpy>

#include "PodcastSource.hpp"
#include "PlayableItem.hpp"
#include "config.h"

using namespace DigitalRooster;

TEST(PodcastSource, dont_add_twice) {
    PodcastSource ps("https://alternativlos.org/alternativlos.rss");
    auto pi = std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi);
    ps.add_episode(pi);
    EXPECT_EQ(ps.get_episodes().size(),1);
}

TEST(PodcastSource, add_two_with_guid) {
    PodcastSource ps("https://alternativlos.org/alternativlos.rss");
    auto pi1 = std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    pi1->set_guid("FooBAR");
    ps.add_episode(pi1);
    auto pi2 = std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi2);
    EXPECT_EQ(ps.get_episodes().size(),2);
}

TEST(PodcastSource, get_episode_names) {
    PodcastSource ps("https://alternativlos.org/alternativlos.rss");
    auto pi = std::make_shared<PodcastEpisode>("TheName", QUrl("http://foo.bar"));
    ps.add_episode(pi);
    EXPECT_EQ(ps.get_episodes_names().size(),1);
    auto ep = ps.get_episodes_names()[0];
    EXPECT_EQ(ep,QString("TheName"));
}

TEST(PodcastSource, set_updater) {
    PodcastSource ps("https://alternativlos.org/alternativlos.rss");
    ps.set_updateInterval(10);
    QSignalSpy spy(&ps, SIGNAL(newDataAvailable()));
    spy.wait(700);
    ASSERT_GT(spy.count(),1);
}


