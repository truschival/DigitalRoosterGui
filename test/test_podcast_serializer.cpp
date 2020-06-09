/*******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 ******************************************************************************/
#include <QDateTime>
#include <QSignalSpy>
#include <QUrl>
#include <QUuid>

#include <system_error>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "PodcastSource.hpp"
#include "appconstants.hpp"
#include "mock_clock.hpp"
#include "podcast_serializer.hpp"
#include "serializer_mock.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;
using ::testing::Invoke;

/******************************************************************************/
class PodcastSourceMock : public DigitalRooster::PodcastSource {
public:
    explicit PodcastSourceMock()
        : PodcastSource(QUrl()){};

    MOCK_CONST_METHOD0(get_cache_file_impl, QString());
    MOCK_CONST_METHOD0(get_description, const QString&());
    MOCK_CONST_METHOD0(get_title, const QString&());
    MOCK_CONST_METHOD0(get_image_url, const QUrl&());
    MOCK_CONST_METHOD0(get_last_updated, const QDateTime&());

    const QString& get_title_nonmock() const {
        return PodcastSource::get_title();
    };
};

/******************************************************************************/
class PodcastSourceMock_episodes : public PodcastSourceMock {
public:
    explicit PodcastSourceMock_episodes(){};
    MOCK_CONST_METHOD0(
        get_episodes_impl, const std::vector<std::shared_ptr<PodcastEpisode>>&());
};

/******************************************************************************/
class SerializerFixture : public ::testing::Test {
public:
    SerializerFixture()
        : cache_dir(DEFAULT_CACHE_DIR_PATH)
        , ps(QUrl()){};

    // Make our own clock to be the wallclock
    void SetUp() {
        mc = std::make_shared<MockClock>();
        DigitalRooster::wallclock =
            std::static_pointer_cast<TimeProvider, MockClock>(mc);
        // Make sure cache path exists
        cache_dir.mkpath(".");
        episode1_json[KEY_URI] = episode1_url.toString();
        episode1_json[KEY_TITLE] = episode1_title;
        episode1_json[KEY_DURATION] = episode1_duration;
        episode1_json[KEY_POSITION] = episode1_position;
        episode1_json[KEY_ID] = episode1_guid;

        episode2_json[KEY_URI] = episode2_url.toString();
        episode2_json[KEY_TITLE] = episode2_title;
        episode2_json[KEY_DURATION] = episode2_duration;
        episode2_json[KEY_POSITION] = episode2_position;
        episode2_json[KEY_ID] = episode2_guid;
    };

    // Restore original TimeProvider for other tests
    void TearDown() {
        DigitalRooster::wallclock = std::make_shared<TimeProvider>();
    };

protected:
    std::shared_ptr<MockClock> mc;
    QDir cache_dir;
    PodcastSourceMock psmock;
    PodcastSource ps;

    QString expected_desc = QString("ExpectedDescription");
    QString expected_title = QString("ExpectedTitle");
    QUrl expected_url = QUrl("http://expected.url");
    QUrl expected_image_url = QUrl("http://expected.image.url/img.png");
    QDateTime expected_timestamp =
        QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate);

    QUrl episode1_url = QString("http://episode1.uri/file1.mp3");
    QUrl episode2_url = QString("http://episode2.uri/file2.mp3");
    QString episode_guid = QString("Some-really-long-guid");
    int episode1_duration = 2000;
    int episode1_position = 1000;
    int episode2_duration = 5000;
    int episode2_position = 0000;
    QString episode1_title = QString("episode1_title");
    QString episode2_title = QString("episode2_title");
    QString episode1_guid = QString("Episode1Guid");
    QString episode2_guid = QString("Episode2Guid");
    QJsonObject episode1_json;
    QJsonObject episode2_json;
};


/******************************************************************************/
TEST_F(SerializerFixture, FileNotFoundThrows) {
    QString test_file_name("/tmp/some_non_existent_file");
    ASSERT_THROW(read_from_file(&psmock, test_file_name), std::system_error);
}

/******************************************************************************/
TEST_F(SerializerFixture, serializePodcastSource) {
    EXPECT_CALL(psmock, get_description())
        .Times(1)
        .WillOnce(ReturnRef(expected_desc));
    EXPECT_CALL(psmock, get_title())
        .Times(1)
        .WillOnce(ReturnRef(expected_title));
    EXPECT_CALL(psmock, get_image_url())
        .Times(1)
        .WillOnce(ReturnRef(expected_image_url));
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(Return(expected_timestamp));

    auto json_obj = json_from_podcast_source(&psmock);
    ASSERT_EQ(json_obj[KEY_DESCRIPTION].toString(), expected_desc);
    ASSERT_EQ(
        json_obj[KEY_TIMESTAMP].toString(), expected_timestamp.toString());
    ASSERT_EQ(json_obj[KEY_ICON_URL].toString(), expected_image_url.toString());
    ASSERT_EQ(json_obj[KEY_TITLE].toString(), expected_title);
}

/******************************************************************************/
TEST_F(SerializerFixture, serializeEpisode) {
    PodcastEpisode episode(episode1_title, episode1_url);
    episode.set_duration(episode1_duration); // duration before position
    episode.set_position(episode1_position); // can't set position if duration=0
    episode.set_guid(episode1_guid);

    auto json_obj = episode.to_json_object();
    ASSERT_EQ(json_obj[KEY_TITLE].toString(), episode1_title);
    ASSERT_EQ(json_obj[KEY_URI].toString(), episode1_url.toString());
    ASSERT_EQ(json_obj[KEY_POSITION].toInt(), episode1_position);
    ASSERT_EQ(json_obj[KEY_DURATION].toInt(), episode1_duration);
    ASSERT_EQ(json_obj[KEY_ID].toString(), episode1_guid);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_PsWasNeverUpdated) {
    QDateTime invalid_date;

    EXPECT_CALL(psmock, get_last_updated())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(invalid_date));

    QJsonObject json_ps;
    json_ps[KEY_TITLE] = expected_title;
    json_ps[KEY_TIMESTAMP] = expected_timestamp.toString();
    parse_podcast_source_from_json(json_ps, &psmock);

    ASSERT_EQ(psmock.get_title_nonmock(), expected_title);
}


/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_PsRecentlyUpdated) {
    // Recently updated PodcastSource with title should not be changed if
    // file is older
    psmock.set_title(expected_title);
    // newer than file
    QDateTime update_timestamp =
        QDateTime::fromString("2018-09-26T08:30:05", Qt::ISODate);

    EXPECT_CALL(psmock, get_last_updated())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(update_timestamp));
    QJsonObject json_ps;
    json_ps[KEY_TITLE] = QString("some_old_title");
    json_ps[KEY_TIMESTAMP] = expected_timestamp.toString();
    parse_podcast_source_from_json(json_ps, &psmock);

    ASSERT_EQ(psmock.get_title_nonmock(), expected_title);
    ASSERT_EQ(psmock.get_episode_count(), 0); // nothing added
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_Add2Episodes) {
    QDateTime invalid_date;

    EXPECT_CALL(psmock, get_last_updated())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(invalid_date));

    QJsonObject json_ps;
    json_ps[KEY_TITLE] = expected_title;
    json_ps[KEY_TIMESTAMP] = expected_timestamp.toString();

    QJsonArray episodes_array;
    episodes_array.append(episode1_json);
    episodes_array.append(episode2_json);
    json_ps[KEY_EPISODES] = episodes_array;

    parse_podcast_source_from_json(json_ps, &psmock);
    read_episodes_cache(json_ps, &psmock);

    ASSERT_EQ(psmock.get_episode_count(), 2);
    ASSERT_EQ(
        psmock.get_episode_by_id(episode1_guid)->get_title(), episode1_title);
    ASSERT_EQ(psmock.get_episode_by_id(episode1_guid)->get_position(),
        episode1_position);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_UpdateEpisodePosition) {
    QDateTime invalid_date;

    EXPECT_CALL(psmock, get_last_updated())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(invalid_date));

    QJsonObject json_ps;
    json_ps[KEY_TITLE] = expected_title;
    json_ps[KEY_TIMESTAMP] = expected_timestamp.toString();
    QJsonArray episodes_array;
    episodes_array.append(episode1_json);
    episode1_json[KEY_POSITION] = 130;
    episodes_array.append(episode1_json);
    json_ps[KEY_EPISODES] = episodes_array;

    parse_podcast_source_from_json(json_ps, &psmock);
    read_episodes_cache(json_ps, &psmock);
    ASSERT_EQ(psmock.get_episode_count(), 1);
    ASSERT_EQ(psmock.get_episode_by_id(episode1_guid)->get_position(), 130);
}

/******************************************************************************/
TEST_F(SerializerFixture, readFromFileInvalidTimestamp) {
    QString test_file_name("some_test_file.json");
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&test_file);
    stream <<  R"({
    	    "timestamp": 5,
    		"url": "https://podcasts.com/some.rss",
    	    "title": "SomeTitle"
    		})";
    test_file.close();
    ASSERT_THROW(read_from_file(&psmock, test_file_name),
        DigitalRooster::PodcastSourceJSonCorrupted);
}

/******************************************************************************/
TEST_F(SerializerFixture, readFromFileDocumentEmpty) {
    QString test_file_name("some_test_file.json");
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&test_file);
    stream << "{}\n";
    test_file.close();

    EXPECT_CALL(psmock, get_cache_file_impl())
        .WillRepeatedly(Return(test_file_name));

    ASSERT_THROW(read_from_file(&psmock, test_file_name),
        DigitalRooster::PodcastSourceJSonCorrupted);

    try {
        read_from_file(&psmock, test_file_name);
    } catch (PodcastSourceJSonCorrupted& exc) {
        EXPECT_STREQ(exc.what(), "Document empty!");
    }
}

/******************************************************************************/
TEST_F(SerializerFixture, readFromFile) {
    QString test_file_name("some_test_file.json");
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
    QTextStream stream(&test_file);
    stream << R"({
             "title": "foo",
             "description": "some fancy description",
             "icon-cached": "/foo/bar/baz.jpg",
             "icon": "http://some.remote.com/baz.jpg",
             "timestamp": "Fr Feb 15 14:21:57 2019",
             "Episodes": [
              { "id": "xx-yyy",
                "duration": 3582395,
                "position": 2314,
                "title": "The First Episode",
                "uri": "http://foo.bar.baz"
              },{ 
                "id": "ZZ-XX-ABV",
                "duration": 3582222,
                "position": 999,
                "title": "Second episode",
                "uri": "http://second_url.baz"
              }
              ]})";
    test_file.close();

    read_from_file(&ps, test_file_name);

    ASSERT_EQ(ps.get_description(), QString("some fancy description"));
    ASSERT_EQ(ps.get_title(), QString("foo"));
    ASSERT_EQ(ps.get_image_url(), QUrl("http://some.remote.com/baz.jpg"));
    ASSERT_EQ(ps.get_image_file_path(), QString("/foo/bar/baz.jpg"));
    ASSERT_EQ(ps.get_episode_count(), 2);
    auto ep = ps.get_episode_by_id("ZZ-XX-ABV");
    ASSERT_TRUE(ep);
    ASSERT_EQ(ep->get_position(), 999);
    ASSERT_EQ(ep->get_duration(), 3582222);
}

/******************************************************************************/
TEST_F(SerializerFixture, fullRoundTrip) {
    PodcastSourceMock_episodes psmock_episodes;
    QString test_file_name("FullRoundTrip_test_file.json");
    std::vector<std::shared_ptr<PodcastEpisode>> ep_vec;
    for (int i = 0; i < 3; i++) {
        auto e = std::make_shared<PodcastEpisode>();
        e->set_duration(1 + i * 1000);
        e->set_position(i);
        e->set_title(QString("Title_%1").arg(i));
        e->set_description(QString("Description_%1").arg(i));
        e->set_guid(QString("http://episode_url/%1").arg(i));
        ep_vec.push_back(e);
    }

    EXPECT_CALL(psmock_episodes, get_cache_file_impl())
        .WillRepeatedly(Return(test_file_name));
    EXPECT_CALL(psmock_episodes, get_description())
        .Times(1)
        .WillOnce(ReturnRef(expected_desc));
    EXPECT_CALL(psmock_episodes, get_title())
        .Times(1)
        .WillOnce(ReturnRef(expected_title));
    EXPECT_CALL(psmock_episodes, get_image_url())
        .Times(1)
        .WillOnce(ReturnRef(expected_image_url));
    EXPECT_CALL(psmock_episodes, get_episodes_impl())
        .Times(1)
        .WillOnce(ReturnRef(ep_vec));

    /* Emulated passing of time */
    EXPECT_CALL(*(mc.get()), get_time())
        .WillOnce(Return(expected_timestamp))
        .WillRepeatedly(Return(expected_timestamp.addSecs(3)));

    store_to_file(&psmock_episodes, test_file_name);
    read_from_file(&ps, test_file_name);

    ASSERT_EQ(ps.get_title(), expected_title);
    ASSERT_EQ(ps.get_description(), expected_desc);
    ASSERT_EQ(ps.get_episode_count(), 3);
    ASSERT_EQ(
        ps.get_episode_by_id(QString("http://episode_url/1"))->get_title(),
        QString("Title_1"));
}

/******************************************************************************/
TEST_F(SerializerFixture, purgeDeletesCacheFile) {
	auto uid = QUuid::fromString(QString("{5c81821d-17fc-44d5-ae45-5ab24ffd1d50}"));
	QString test_file_name =
        cache_dir.filePath(uid.toString(QUuid::WithoutBraces));
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&test_file);
    stream << R"(
    {
    "id": "{5c81821d-17fc-44d5-ae45-5ab24ffd1d50}",
    "description": "Some Description",
	"icon": "https://some.remote.url/test.jpg",
    "icon-cached": "/tmp/local_cache/foo.jpg",
    "timestamp": "Thu Nov 14 19:48:55 2019",
	"url": "https://alternativlos.org/alternativlos.rss",
    "title": "MyTitle1"
	})";
    test_file.close();
    ASSERT_TRUE(test_file.exists());

    PodcastSource source(QUrl("https://alternativlos.org/alternativlos.rss"),uid);
    auto serializer = std::make_unique<PodcastSerializer>(
        cache_dir, &source, std::chrono::milliseconds(50));

    serializer->restore_info(); // populate podcastSource
    source.set_serializer(std::move(serializer));
    ASSERT_EQ(source.get_title(), QString("MyTitle1"));
    ASSERT_TRUE(test_file.exists());
    source.purge();
    ASSERT_FALSE(test_file.exists());
}

/******************************************************************************/
TEST_F(SerializerFixture, badCacheNoThrow) {
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(Return(expected_timestamp));

    QDir cache_dir_bad("/some/nonexistent/cache/dir");
    auto serializer = std::make_unique<PodcastSerializer>(
        cache_dir_bad, &ps, std::chrono::seconds(1));
    ASSERT_NO_THROW(serializer->restore_info());
    ps.set_title("NewTitle");
    ASSERT_NO_THROW(serializer->write());
}
/******************************************************************************/
TEST(Serializer, parseBadTimestamp) {
    QString json_string(R"(
	{
    "id": "{5c81821d-17fc-44d5-ae45-5ab24ffd1d50}",
    "description": "Some Description",
	"icon": "https://some.remote.url/test.jpg",
    "icon-cached": "/tmp/local_cache/foo.jpg",
    "timestamp": "19:48:55 13.05.2019",
    "title": "MyTitle"
	})");
    PodcastSource ps(QUrl("http://some.url"));
    auto jdoc = QJsonDocument::fromJson(json_string.toUtf8());
    EXPECT_THROW(parse_podcast_source_from_json(jdoc.object(), &ps),
        PodcastSourceJSonCorrupted);
}
/******************************************************************************/
