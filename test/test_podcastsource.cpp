// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
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
        , ps(QUrl("https://alternativlos.org/alternativlos.rss"), uid)
        , image_url("https://raw.githubusercontent.com/truschival/"
                    "DigitalRoosterGui/develop/test/old_icon.png")
        , cache_file(
              cache_dir.filePath(uid.toString(QUuid::WithoutBraces) + ".png")) {
        ps.set_max_episodes(5);
        ep1 = std::make_shared<PodcastEpisode>("Name1",QUrl("http://foo-1.bar"));
        ep1->set_publication_date(QDateTime::fromSecsSinceEpoch(100000000));
        ep2 = std::make_shared<PodcastEpisode>("Name2",QUrl("http://foo-2.bar"));
        ep2->set_publication_date(QDateTime::fromSecsSinceEpoch(200000000));
        ep3 = std::make_shared<PodcastEpisode>("Name3",QUrl("http://foo-3.bar"));
        ep3->set_publication_date(QDateTime::fromSecsSinceEpoch(300000000));
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
    QUrl image_url;
    QFile cache_file;
    /* common episodes */
    std::shared_ptr<PodcastEpisode> ep1;
    std::shared_ptr<PodcastEpisode> ep2;
    std::shared_ptr<PodcastEpisode> ep3;
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
TEST_F(PodcastSourceFixture, expected_order) {
    ps.add_episode(ep2);
    ps.add_episode(ep3);
    ps.add_episode(ep1);
    EXPECT_EQ(ps.get_episodes().size(), 3);
    // oldest last
    EXPECT_EQ(ps.get_episodes().back()->get_url(),ep1->get_url());
    // ep3 first
    EXPECT_EQ(ps.get_episodes().front()->get_url(),ep3->get_url());
}


/******************************************************************************/
TEST_F(PodcastSourceFixture, max_reached_adding) {
    ps.set_max_episodes(3);
    auto p4 =
        std::make_shared<PodcastEpisode>("Name4", QUrl("http://foo-4.bar"));
    // episode 4 is published between ep2 and ep3
    p4->set_publication_date(QDateTime::fromSecsSinceEpoch(220000000));
    ps.add_episode(ep1);
    ps.add_episode(ep2);
    ps.add_episode(ep3);
    ps.add_episode(p4);
    // expect ep1 to be removed
    EXPECT_EQ(ps.get_episodes().size(), 3);
    // now p2 is oldest
    EXPECT_EQ(ps.get_episodes().back()->get_url(),ep2->get_url());
    // ep3 first
    EXPECT_EQ(ps.get_episodes().front()->get_url(),ep3->get_url());
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
    ps.set_image_url(image_url);
    ASSERT_EQ(ps.get_icon(), image_url);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, returnLocalIconIfStored) {
    cache_file.open(QIODevice::WriteOnly);
    cache_file.write("foo");
    cache_file.close();
    ASSERT_TRUE(cache_file.exists());

    ps.set_image_url(image_url);
    ps.set_image_file_path(cache_file.fileName());

    ASSERT_EQ(ps.get_icon(), QUrl::fromLocalFile(cache_file.fileName()));
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, setIconUrlTriggersDownload) {
    // For downloading cache PodcastSource needs a Serializer
    ps.set_serializer(std::make_unique<PodcastSerializer>(cache_dir, &ps));
    QSignalSpy spy(&ps, SIGNAL(icon_changed()));
    ps.set_image_url(image_url);
    // First time return image_url - data not yet cached
    ASSERT_EQ(ps.get_icon(), image_url);
    spy.wait(); // after download icon_changed() is emitted

    auto expected_icon_url = QUrl::fromLocalFile(
        cache_dir.filePath(ps.create_image_file_name(image_url)));
    // Second time the local cache should be returned
    ASSERT_EQ(ps.get_icon(), expected_icon_url);
    QFile iconcache(cache_dir.filePath(ps.create_image_file_name(image_url)));

    ASSERT_TRUE(iconcache.exists());
    ASSERT_TRUE(iconcache.remove());
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, updateWithNewIconTriggersDownload) {
    QSignalSpy spy(&ps, SIGNAL(icon_changed()));
    ps.set_serializer(std::make_unique<PodcastSerializer>(cache_dir, &ps));
    ps.set_image_url(image_url); // emits but we don't care
    spy.wait(10);
    spy.wait(); // after download icon_changed() is emitted
    QFile iconcache(cache_dir.filePath(ps.create_image_file_name(image_url)));
    ASSERT_TRUE(iconcache.exists());

    auto image_url_new = QUrl("https://raw.githubusercontent.com/truschival/"
                              "DigitalRoosterGui/develop/test/new_icon.png");
    ps.set_image_url(image_url_new);
    spy.wait(10);
    spy.wait();
    auto expected_icon_url = QUrl::fromLocalFile(
        cache_dir.filePath(ps.create_image_file_name(image_url_new)));

    // Second time the local cache should be returned
    ASSERT_EQ(ps.get_icon(), expected_icon_url);
    QFile iconcache_new(
        cache_dir.filePath(ps.create_image_file_name(image_url_new)));
    ASSERT_TRUE(iconcache_new.exists());
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, cacheFileName) {
    auto hash_old_icon = QString("6e7f6f4a4b0414455d81dfb68e4e9dac");
    auto expected = QString(ps.get_id_string() + "_" + hash_old_icon + ".png")
                        .toStdString();
    ASSERT_EQ(ps.create_image_file_name(image_url).toStdString(), expected);
}

/******************************************************************************/
TEST_F(PodcastSourceFixture, purgeIconCache) {
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
