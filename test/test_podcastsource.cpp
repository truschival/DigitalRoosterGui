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
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <stdexcept> // std::system_error

#include <QSignalSpy>
#include <QString>
#include <QUrl>

#include "PlayableItem.hpp"
#include "PodcastSource.hpp"
#include "appconstants.hpp"

#include "serializer_mock.hpp"

using namespace DigitalRooster;

/******************************************************************************/

class PodcastSourceFixture : public virtual ::testing::Test {
public:
    PodcastSourceFixture()
        : cache_dir(DEFAULT_CACHE_DIR_PATH)
        , uid(QUuid::createUuid())
        , ps(QUrl("https://alternativlos.org/alternativlos.rss"), uid) {
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
    QSignalSpy spy_data(&ps, SIGNAL(dataChanged()));
    ASSERT_TRUE(spy.isValid());
    ASSERT_TRUE(spy_data.isValid());

    ps.add_episode(pi1);

    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(spy_data.count(), 0); // should not emit dataChanged
    // arguments of first signal, here only 1 int
    auto arguments = spy.takeFirst();
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
    QString expected_filename(uid.toString());
    ASSERT_EQ(filename, expected_filename);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, emit_Description_Title_Changed) {
    QSignalSpy spy_desc(&ps, SIGNAL(descriptionChanged()));
    QSignalSpy spy_data(&ps, SIGNAL(dataChanged()));
    QSignalSpy spy_title(&ps, SIGNAL(titleChanged()));
    ASSERT_TRUE(spy_desc.isValid());
    ASSERT_TRUE(spy_data.isValid());
    ASSERT_TRUE(spy_title.isValid());

    ps.set_title("NewTitle");
    ps.set_description("MyDescription");

    ASSERT_EQ(spy_desc.count(), 1);
    ASSERT_EQ(spy_data.count(), 2);
    ASSERT_EQ(spy_title.count(), 1);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, returnIconUrlIfNotStored) {
    auto image_url = QUrl(
        "https://www.ruschival.de/wp-content/uploads/2013/12/424504199718.jpg");
    ps.set_image_url(image_url);
    ASSERT_EQ(ps.get_icon(), image_url);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, returnLocalIconIfStored) {
    auto image_url = QUrl(
        "https://www.ruschival.de/wp-content/uploads/2013/12/424504199718.jpg");
    auto file_name = image_url.fileName();
    QFile cache_file(cache_dir.filePath(file_name));
    cache_file.open(QIODevice::WriteOnly);
    cache_file.write("foo");
    cache_file.close();
    ASSERT_TRUE(cache_file.exists());

    ps.set_image_url(image_url);
    ps.set_image_file_path(cache_dir.filePath(file_name));

    ASSERT_EQ(
        ps.get_icon(), QUrl::fromLocalFile(cache_dir.filePath(file_name)));
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, setIconUrlTriggersDownload) {
    auto image_url = QUrl("https://www.ruschival.de/wp-content/uploads/2013/12/"
                          "10150369594129719-150x150.jpg");
    auto file_name = image_url.fileName();
    QFile cache_file(cache_dir.filePath(file_name));
    // For downloading cache PodcastSource needs a Serializer
    ps.set_serializer(std::make_unique<PodcastSerializer>(cache_dir, &ps));
    QSignalSpy spy(&ps, SIGNAL(icon_changed()));
    ps.set_image_url(image_url);
    // First time return image_url - data not yet cached
    ASSERT_EQ(ps.get_icon(), image_url);
    spy.wait(); // after download icon_changed() is emitted
    ASSERT_EQ(spy.count(), 2);
    // Second time the local cache should be returned
    auto expeced_local_url = QUrl::fromLocalFile(cache_dir.filePath(file_name));
    ASSERT_EQ(ps.get_icon(), expeced_local_url);

    ASSERT_TRUE(cache_file.exists());
    ASSERT_TRUE(cache_file.remove());
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, updateIconUrlTriggersDownload) {
    auto image_url = QUrl("https://www.ruschival.de/wp-content/uploads/2013/12/"
                          "424523119718-150x150.jpg");
    auto file_name = image_url.fileName();

    auto image_url_new = QUrl("https://www.ruschival.de/wp-content/uploads/"
                              "2013/12/10150369594129719-150x150.jpg");
    auto file_name_new = image_url_new.fileName();


    QSignalSpy spy(&ps, SIGNAL(icon_changed()));
    ps.set_serializer(std::make_unique<PodcastSerializer>(cache_dir, &ps));
    ps.set_image_url(image_url); // emits but we don't care
    spy.wait();
    spy.wait(); // after download icon_changed() is emitted

    ps.set_image_url(image_url_new); // emits
    spy.wait();                      // set_image_url(image_url_new)
    spy.wait(); // set_image_file_path() with new cache file
    ASSERT_EQ(spy.count(), 4);
    // Second time the local cache should be returned
    auto expected_local_url =
        QUrl::fromLocalFile(cache_dir.filePath(file_name_new));
    ASSERT_EQ(ps.get_icon(), expected_local_url);

    QFile cache_file(cache_dir.filePath(file_name));
    ASSERT_FALSE(cache_file.exists()); // Should have been deleted when updated

    QFile cache_file_new(cache_dir.filePath(file_name_new));
    ASSERT_TRUE(cache_file_new.exists());
    ASSERT_TRUE(cache_file_new.remove());
}


/******************************************************************************/
TEST_F(PodcastSourceFixture, purgeIconCache) {
    auto image_url = QUrl(
        "https://www.ruschival.de/wp-content/uploads/2013/12/424504199718.jpg");
    auto file_name = image_url.fileName();
    QFile cache_file(cache_dir.filePath(file_name));
    cache_file.open(QIODevice::WriteOnly);
    cache_file.write("foo");
    cache_file.close();
    ASSERT_TRUE(cache_file.exists());
    ps.set_image_url(image_url);
    ps.set_image_file_path(cache_dir.filePath(file_name));
    // delete icon cache
    ps.purge_icon_cache();

    ASSERT_FALSE(cache_file.exists());   // file is deleted
    ASSERT_EQ(ps.get_icon(), image_url); // URL not local cache
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, purgeWithSerializer) {
    auto serializer = std::make_unique<SerializerMock>(cache_dir, &ps);
    EXPECT_CALL(*(serializer.get()), delete_cache()).Times(1);
    QSignalSpy spy(&ps, SIGNAL(episodes_count_changed(int)));
    ps.set_serializer(std::move(serializer));
    ps.purge();
    EXPECT_EQ(spy.count(), 1);
    auto argslist = spy.takeFirst();
    EXPECT_EQ(argslist[0].toInt(), 0);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, episodePositionChangedTriggersDelayedWrite) {
    auto serializer = std::make_unique<SerializerMock>(
        cache_dir, &ps, std::chrono::milliseconds(50));
    EXPECT_CALL(*(serializer.get()), write_cache()).Times(1);
    QSignalSpy spy(&ps, SIGNAL(dataChanged()));
    ps.set_serializer(std::move(serializer));
    auto episode = std::make_shared<PodcastEpisode>(
        "TestEpisode", QUrl("http://some.url"));
    episode->set_duration(100000);
    ps.add_episode(episode);
    episode->set_position(4000);
    spy.wait(1000);
    EXPECT_EQ(spy.count(), 1);
}

/******************************************************************************/
TEST(PodcastSource, fromGoodJson) {
    QString json_string(R"(
	{
    "id": "{5c81821d-17fc-44d5-ae45-5ab24ffd1d50}",
    "description": "Some Description",
	"icon": "https://some.remote.url/test.jpg",
    "icon-cached": "/tmp/local_cache/foo.jpg",
    "timestamp": "Thu Nov 14 19:48:55 2019",
	"url": "https://alternativlos.org/alternativlos.rss",
    "title": "MyTitle"
	})");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    auto ps = PodcastSource::from_json_object(jdoc.object());
    EXPECT_EQ(ps->get_id(), QString("5c81821d-17fc-44d5-ae45-5ab24ffd1d50}"));
    EXPECT_EQ(ps->get_title(), QString("MyTitle"));
    EXPECT_EQ(
        ps->get_url(), QUrl("https://alternativlos.org/alternativlos.rss"));
    EXPECT_EQ(ps->get_description(), QString("Some Description"));
    EXPECT_EQ(ps->get_icon(), QString("https://some.remote.url/test.jpg"));
}

/******************************************************************************/
TEST(PodcastSource, fromBadJson) {
    QString json_string(R"(
	{ 
	"aKeyWithoutValue"
	})");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    EXPECT_THROW(
        PodcastSource::from_json_object(jdoc.object()), std::invalid_argument);
}

/******************************************************************************/
TEST(PodcastSource, fromBadJsonInvalidUrl) {
    QString json_string(R"(
	{
    "id": "{5c81821d-17fc-44d5-ae45-5ab24ffd1d50}",
    "description": "Some Description",
	"icon": "https://some.remote.url/test.jpg",
    "icon-cached": "/tmp/local_cache/foo.jpg",
    "timestamp": "Thu Nov 14 19:48:55 2019",
    "title": "MyTitle"
	})");
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    EXPECT_THROW(
        PodcastSource::from_json_object(jdoc.object()), std::invalid_argument);
}
