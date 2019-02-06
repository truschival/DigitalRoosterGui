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
    PodcastSourceMock()
        : PodcastSource(){

          };

    MOCK_CONST_METHOD0(get_cache_file_impl, QString());
    MOCK_CONST_METHOD0(get_description, const QString&());
    MOCK_CONST_METHOD0(get_title, const QString&());
    MOCK_CONST_METHOD0(get_image_uri, const QUrl&());
    MOCK_CONST_METHOD0(get_last_updated, const QDateTime&());

    MOCK_CONST_METHOD1(
        get_episode_by_id_impl, std::shared_ptr<PodcastEpisode>(QString));

    const QString& get_title_nonmock() const {
        return PodcastSource::get_title();
    };
};

/******************************************************************************/
class SerializerFixture : public ::testing::Test {
public:
    // Make our own clock to be the wallclock
    void SetUp() {
        mc = std::make_shared<MockClock>();
        DigitalRooster::wallclock =
            std::static_pointer_cast<TimeProvider, MockClock>(mc);

        episode1_json[KEY_URI] = QString("http://episode1.uri/file1.mp3");
        episode1_json[KEY_NAME] = QString("episode1_name");
        episode1_json[KEY_DURATION] = 5000;
        episode1_json[KEY_POSITION] = 0;
        episode1_json[KEY_ID] = QString("Episode1Guid");

        episode2_json[KEY_URI] = QString("http://episode2.uri/file2.mp3");
        episode2_json[KEY_NAME] = QString("episode2_name");
        episode2_json[KEY_DURATION] = 4000;
        episode2_json[KEY_POSITION] = 500;
        episode2_json[KEY_ID] = QString("Episode2Guid");
    };

    // Restore original TimeProvider for other tests
    void TearDown() {
        DigitalRooster::wallclock = std::make_shared<TimeProvider>();
    };

protected:
    std::shared_ptr<MockClock> mc;

    QString expected_desc = QString("ExpectedDescription");
    QString expected_title = QString("ExpectedTitle");
    QUrl expected_url = QUrl("http://expected.url");
    QUrl expected_image_url = QUrl("http://expected.image.url/img.png");
    QDateTime expected_timestamp =
        QDateTime::fromString("2018-09-26T08:29:45", Qt::ISODate);

    QString episode_name = QString("AnEpisodeName");
    QUrl episode_url = QUrl("http://episode.url.com/somefile.mp4");
    QString episode_guid = QString("Some-really-long-guid");
    int episode_duration = 2000;
    int episode_position = 1000;

    QJsonObject episode1_json;
    QJsonObject episode2_json;
};


/******************************************************************************/
TEST(PodcastSerializer, FileNotFoundThrows) {
    PodcastSerializer dut;
    PodcastSourceMock psmock;
    EXPECT_CALL(psmock, get_cache_file_impl())
        .Times(1)
        .WillOnce(Return(QString("/tmp/some_non_existent_file")));
    ASSERT_THROW(dut.read_from_file(&psmock), std::system_error);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceSerialization) {
    PodcastSerializer dut;
    PodcastSourceMock psmock;

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
        json_obj[KEY_IMAGE_PATH].toString(), expected_image_url.toString());
    ASSERT_EQ(json_obj[KEY_TITLE].toString(), expected_title);
}
/******************************************************************************/
TEST_F(SerializerFixture, PodcastEpisodeSerialization) {
    PodcastSerializer dut;
    PodcastEpisode episode(episode_name, episode_url);
    episode.set_duration(episode_duration); // duration before position
    episode.set_position(episode_position); // can't set position if duration=0
    episode.set_guid(episode_guid);

    auto json_obj = dut.json_from_episode(&episode);
    ASSERT_EQ(json_obj[KEY_NAME].toString(), episode_name);
    ASSERT_EQ(json_obj[KEY_URI].toString(), episode_url.toString());
    ASSERT_EQ(json_obj[KEY_POSITION].toInt(), episode_position);
    ASSERT_EQ(json_obj[KEY_DURATION].toInt(), episode_duration);
    ASSERT_EQ(json_obj[KEY_ID].toString(), episode_guid);
}

/******************************************************************************/
TEST_F(SerializerFixture, PodcastSourceFromJson_PsWasNeverUpdated) {
    PodcastSerializer dut;
    PodcastSourceMock psmock;
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
    PodcastSourceMock psmock;
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
    PodcastSourceMock psmock;
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
}

/******************************************************************************/
