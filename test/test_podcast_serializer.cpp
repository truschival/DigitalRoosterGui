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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mock_clock.hpp"

#include "PodcastSource.hpp"
#include "appconstants.hpp"
#include "podcast_serializer.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;
using ::testing::Invoke;

/******************************************************************************/
class PodcastSourceMock : public DigitalRooster::PodcastSource {
public:
    explicit PodcastSourceMock(const QDir& cachedir)
        : PodcastSource(QUrl(), cachedir){};

    MOCK_CONST_METHOD0(get_cache_file_impl, QString());
    MOCK_CONST_METHOD0(get_description, const QString&());
    MOCK_CONST_METHOD0(get_title, const QString&());
    MOCK_CONST_METHOD0(get_image_uri, const QUrl&());
    MOCK_CONST_METHOD0(get_last_updated, const QDateTime&());

    const QString& get_title_nonmock() const {
        return PodcastSource::get_title();
    };
};

/******************************************************************************/
class PodcastSourceMock_episodes : public PodcastSourceMock {
public:
    explicit PodcastSourceMock_episodes(const QDir& cachedir)
        : PodcastSourceMock(cachedir){};

    MOCK_CONST_METHOD0(
        get_episodes_impl, const QVector<std::shared_ptr<PodcastEpisode>>&());
};

/******************************************************************************/
class SerializerFixture : public ::testing::Test {
public:
    SerializerFixture()
        : cache_dir(
              TEST_FILE_PATH + QDir::separator() + QString("podcast_cache"))
        , psmock(cache_dir)
        , ps(QUrl(), cache_dir){};

    // Make our own clock to be the wallclock
    void SetUp() {
        mc = std::make_shared<MockClock>();
        DigitalRooster::wallclock =
            std::static_pointer_cast<TimeProvider, MockClock>(mc);

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
    PodcastSerializer dut;
    EXPECT_CALL(psmock, get_cache_file_impl())
        .Times(1)
        .WillOnce(Return(QString("/tmp/some_non_existent_file")));
    ASSERT_THROW(dut.read_from_file(&psmock), std::system_error);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceSerialization) {
    PodcastSerializer dut;
    EXPECT_CALL(psmock, get_description())
        .Times(1)
        .WillOnce(ReturnRef(expected_desc));
    EXPECT_CALL(psmock, get_title())
        .Times(1)
        .WillOnce(ReturnRef(expected_title));
    EXPECT_CALL(psmock, get_image_uri())
        .Times(1)
        .WillOnce(ReturnRef(expected_image_url));
    EXPECT_CALL(*(mc.get()), get_time())
        .Times(1)
        .WillOnce(Return(expected_timestamp));

    auto json_obj = dut.json_from_podcast_source(&psmock);
    ASSERT_EQ(json_obj[KEY_DESCRIPTION].toString(), expected_desc);
    ASSERT_EQ(
        json_obj[KEY_TIMESTAMP].toString(), expected_timestamp.toString());
    ASSERT_EQ(
        json_obj[KEY_ICON_URL].toString(), expected_image_url.toString());
    ASSERT_EQ(json_obj[KEY_TITLE].toString(), expected_title);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastEpisodeSerialization) {
    PodcastSerializer dut;
    PodcastEpisode episode(episode1_title, episode1_url);
    episode.set_duration(episode1_duration); // duration before position
    episode.set_position(episode1_position); // can't set position if duration=0
    episode.set_guid(episode1_guid);

    auto json_obj = dut.json_from_episode(&episode);
    ASSERT_EQ(json_obj[KEY_TITLE].toString(), episode1_title);
    ASSERT_EQ(json_obj[KEY_URI].toString(), episode1_url.toString());
    ASSERT_EQ(json_obj[KEY_POSITION].toInt(), episode1_position);
    ASSERT_EQ(json_obj[KEY_DURATION].toInt(), episode1_duration);
    ASSERT_EQ(json_obj[KEY_ID].toString(), episode1_guid);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_PsWasNeverUpdated) {
    PodcastSerializer dut;
    QDateTime invalid_date;

    EXPECT_CALL(psmock, get_last_updated())
        .Times(AtLeast(1))
        .WillRepeatedly(ReturnRef(invalid_date));

    QJsonObject json_ps;
    json_ps[KEY_TITLE] = expected_title;
    json_ps[KEY_TIMESTAMP] = expected_timestamp.toString();
    dut.parse_podcast_source_from_json(json_ps, &psmock);

    ASSERT_EQ(psmock.get_title_nonmock(), expected_title);
}


/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_PsRecentlyUpdated) {
    PodcastSerializer dut;
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
    dut.parse_podcast_source_from_json(json_ps, &psmock);

    ASSERT_EQ(psmock.get_title_nonmock(), expected_title);
    ASSERT_EQ(psmock.get_episode_count(), 0); // nothing added
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_Add2Episodes) {
    PodcastSerializer dut;
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

    dut.parse_podcast_source_from_json(json_ps, &psmock);
    ASSERT_EQ(psmock.get_episode_count(), 2);
    ASSERT_EQ(
        psmock.get_episode_by_id(episode1_guid)->get_title(), episode1_title);
    ASSERT_EQ(psmock.get_episode_by_id(episode1_guid)->get_position(),
        episode1_position);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_UpdateEpisodePosition) {
    PodcastSerializer dut;
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

    dut.parse_podcast_source_from_json(json_ps, &psmock);
    ASSERT_EQ(psmock.get_episode_count(), 1);
    ASSERT_EQ(psmock.get_episode_by_id(episode1_guid)->get_position(), 130);
}

/******************************************************************************/
TEST_F(SerializerFixture, ReadFromFileInvalidTimestamp) {
    PodcastSerializer dut;
    QString test_file_name("some_test_file.json");
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&test_file);
    stream << "{ \n"
              " \"title\": \"foo\", \n"
              " \"timestamp\": 3, \n" /*note: invalid timestamp*/
              " \"Episodes\": [ \n"
              "   { \n"
              "     \"id\": \"xx-yyy\", \n"
              "     \"duration\": 3582395,\n"
              "     \"position\": 2314, \n"
              "     \"title\": \"The First Episode\", \n"
              "     \"uri\": \"http://foo.bar.baz\" \n"
              "   } \n"
              " ] \n"
              "} \n";
    test_file.close();

    EXPECT_CALL(psmock, get_cache_file_impl())
        .Times(1)
        .WillOnce(Return(test_file_name));
    ASSERT_THROW(dut.read_from_file(&psmock),
        DigitalRooster::PodcastSourceJSonCorrupted);
}

/******************************************************************************/
TEST_F(SerializerFixture, ReadFromFileDocumentEmpty) {
    PodcastSerializer dut;
    QString test_file_name("some_test_file.json");
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&test_file);
    stream << "{}\n";
    test_file.close();

    EXPECT_CALL(psmock, get_cache_file_impl())
        .WillRepeatedly(Return(test_file_name));

    ASSERT_THROW(dut.read_from_file(&psmock),
        DigitalRooster::PodcastSourceJSonCorrupted);

    try {
        dut.read_from_file(&psmock);
    } catch (PodcastSourceJSonCorrupted& exc) {
        EXPECT_STREQ(exc.what(), "Document empty!");
    }
}

/******************************************************************************/
TEST_F(SerializerFixture, ReadFromFile) {
    PodcastSerializer dut;
    QString test_file_name("some_test_file.json");
    /* create file to read */
    QFile test_file(test_file_name);
    test_file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&test_file);
    stream << "{ \n"
              " \"title\": \"foo\", \n"
              " \"description\": \"some fancy description\", \n"
              " \"image\": \"/foo/bar/baz.jpg\",\n"
              " \"timestamp\": \"Fr Feb 15 14:21:57 2019\", \n"
              " \"Episodes\": [ \n"
              "   { \n"
              "     \"id\": \"xx-yyy\", \n"
              "     \"duration\": 3582395,\n"
              "     \"position\": 2314, \n"
              "     \"title\": \"The First Episode\", \n"
              "     \"uri\": \"http://foo.bar.baz\" \n"
              "   }, \n"
              "   { \n"
              "     \"id\": \"ZZ-XX-ABV\", \n"
              "     \"duration\": 3582222, \n"
              "     \"position\": 999, \n"
              "     \"title\": \"Second episode\", \n"
              "     \"uri\": \"http://second_url.baz\" \n"
              "   } \n"
              " ] \n"
              "} \n";
    test_file.close();

    dut.read_from_file(&ps, test_file_name);

    ASSERT_EQ(ps.get_description(), QString("some fancy description"));
    ASSERT_EQ(ps.get_title(), QString("foo"));
    ASSERT_EQ(ps.get_image_uri(), QUrl("/foo/bar/baz.jpg"));
    ASSERT_EQ(ps.get_episode_count(), 2);
    auto ep = ps.get_episode_by_id("ZZ-XX-ABV");
    ASSERT_TRUE(ep);
    ASSERT_EQ(ep->get_position(), 999);
    ASSERT_EQ(ep->get_duration(), 3582222);
}

/******************************************************************************/
TEST_F(SerializerFixture, FullRoundTrip) {
    PodcastSerializer dut;
    PodcastSourceMock_episodes psmock_episodes(cache_dir);
    QString test_file_name("FullRoundTrip_test_file.json");
    QVector<std::shared_ptr<PodcastEpisode>> ep_vec;
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
    EXPECT_CALL(psmock_episodes, get_image_uri())
        .Times(1)
        .WillOnce(ReturnRef(expected_image_url));
    EXPECT_CALL(psmock_episodes, get_episodes_impl())
        .Times(1)
        .WillOnce(ReturnRef(ep_vec));

    /* Emulated passing of time */
    EXPECT_CALL(*(mc.get()), get_time())
        .WillOnce(Return(expected_timestamp))
        .WillRepeatedly(Return(expected_timestamp.addSecs(3)));

    dut.store_to_file(&psmock_episodes);
    dut.read_from_file(&ps, test_file_name);

    ASSERT_EQ(ps.get_title(), expected_title);
    ASSERT_EQ(ps.get_description(), expected_desc);
    ASSERT_EQ(ps.get_episode_count(), 3);
    ASSERT_EQ(
        ps.get_episode_by_id(QString("http://episode_url/1"))->get_title(),
        QString("Title_1"));
}

/******************************************************************************/
