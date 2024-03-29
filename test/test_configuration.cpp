// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QJsonDocument>
#include <QSettings>
#include <QSignalSpy>
#include <QStandardPaths>

#include "gtest/gtest.h"

#include "PlayableItem.hpp"
#include "PodcastSource.hpp"
#include "alarm.hpp"
#include "appconstants.hpp"
#include "configuration.hpp"

using namespace DigitalRooster;

class ConfigurationFixture : public virtual ::testing::Test {
public:
    ConfigurationFixture()
        : filename(DEFAULT_CONFIG_FILE_PATH + "_SettingsFixture")
        , cache_dir(DEFAULT_CACHE_DIR_PATH) {
    }

    ~ConfigurationFixture() {
    }

    void SetUp() {
        add_podcast_sources(appconfig);
        add_internet_radio(appconfig);
        add_alarms(appconfig);
        add_weather_config(appconfig);
        QJsonDocument doc(appconfig);
        QFile tf(filename);
        QFileInfo file_info(tf);
        QDir().mkpath(file_info.path());
        tf.open(QIODevice::ReadWrite | QIODevice::Text);
        tf.write(doc.toJson());
        tf.close();

        config = std::make_unique<Configuration>(filename, cache_dir);
        config->update_configuration();
    }

    void TearDown() {
        QFileInfo file_info(filename);
        QDir(file_info.dir()).removeRecursively();

        QDir cd(cache_dir);
        cd.removeRecursively();
    }

protected:
    QString filename;
    QString cache_dir;
    QJsonObject appconfig;
    std::unique_ptr<Configuration> config;

    void add_internet_radio(QJsonObject& root) {
        QJsonArray radiosources;
        QJsonObject dradio;
        dradio[KEY_NAME] = "DRadio";
        dradio[KEY_ID] = "a754affb-fd4b-4825-9eba-32b64fd7d50c";
        dradio[KEY_URI] = "http://dradio.de";
        radiosources.append(dradio);

        QJsonObject swr2;
        swr2[KEY_NAME] = "SWR2";
        swr2[KEY_ID] = "c98a9c37-f990-47eb-ab73-21daf48d43c0";
        swr2[KEY_URI] = "http://swr2.de";
        radiosources.append(swr2);

        QJsonObject badStation;
        badStation[KEY_NAME] = "BadStation";
        badStation[KEY_ID] = "Not An UUID!";
        badStation[KEY_URI] = "http://swr2.de";
        radiosources.append(badStation);

        root[KEY_GROUP_IRADIO_SOURCES] = radiosources;
    }

    void add_podcast_sources(QJsonObject& root) {
        QJsonArray pcsources;
        QJsonObject pc1;
        pc1[KEY_NAME] = "Alternativlos";
        pc1[KEY_ID] = "ab25693c-aae1-40dc-8946-8d2b670d4598";
        pc1[KEY_URI] = "https://alternativlos.org/alternativlos.rss";
        pcsources.append(pc1);

        QJsonObject pc2;
        pc2[KEY_NAME] = "Dradio Essay&Diskurs";
        pc2[KEY_ID] = "a754affb-fd4b-4825-9eba-32b64fd7d50c";
        pc2[KEY_URI] = "http://www.deutschlandfunk.de/"
                       "podcast-essay-und-diskurs.1185.de.podcast.xml";
        pcsources.append(pc2);

        QJsonObject pc3;
        pc3[KEY_NAME] = "Bad Station";
        pc3[KEY_ID] = "a754affb-fd4b-4825-9eba-32b64fd7d50c";
        pc3[KEY_URI] = "Not An URL - Throws!";
        pcsources.append(pc3);

        root[KEY_GROUP_PODCAST_SOURCES] = pcsources;
    }

    void add_alarms(QJsonObject& root) {
        QJsonArray alarms;
        QJsonObject al1;
        al1[JSON_KEY_TIME] = "10:00";
        al1[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al1[KEY_ALARM_PERIOD] = "daily";
        al1[KEY_ENABLED] = true;
        al1[KEY_ID] = "1a4bf6bd-7e67-4b40-80fd-b13e2524fc74";
        alarms.append(al1);

        QJsonObject al2;
        al2[JSON_KEY_TIME] = "07:00";
        al2[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al2[KEY_ALARM_PERIOD] = "workdays";
        al2[KEY_ENABLED] = true;
        al2[KEY_ID] = "12eb4390-6abf-4626-be48-f11fe20f45cf";
        alarms.append(al2);

        QJsonObject al3;
        al3[JSON_KEY_TIME] = "09:00";
        al3[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al3[KEY_ALARM_PERIOD] = "weekend";
        al3[KEY_ENABLED] = false;
        al3[KEY_ID] = "62ab05d7-d9ab-4254-8bfd-47bfdc74417a";
        alarms.append(al3);

        QJsonObject al4;
        al4[JSON_KEY_TIME] = "13:00";
        al4[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al4[KEY_ALARM_PERIOD] = "once";
        al4[KEY_ENABLED] = true;
        al4[KEY_ID] = "fa3ce587-ab02-4328-9c68-4ee5e3626c86";
        alarms.append(al4);

        QJsonObject al5;
        al5[JSON_KEY_TIME] = "17:00";
        al5[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al5[KEY_ALARM_PERIOD] = "Manchmal";
        al5[KEY_ENABLED] = true;
        al5[KEY_ID] = "694485e9-ac44-46f5-bc45-730a7a0ac387";
        alarms.append(al5);

        QJsonObject al6;
        al6[JSON_KEY_TIME] = "25:34";
        al6[KEY_URI] = "http://st01.dlf.de/dlf/01/128/mp3/stream.mp3";
        al6[KEY_ALARM_PERIOD] = "once";
        al6[KEY_ENABLED] = true;
        al6[KEY_ID] = "694485e9-ac44-46f5-bc45-730a7a0a2387";
        alarms.append(al6);

        QJsonObject al7;
        al7[JSON_KEY_TIME] = "12:34";
        al7[KEY_URI] = "";
        al7[KEY_ALARM_PERIOD] = "once";
        al7[KEY_ENABLED] = true;
        al7[KEY_ID] = "694485e9-ac44-46f5-bc45-730a7a0a2387";
        alarms.append(al7);

        QJsonObject al8;
        al8[JSON_KEY_TIME] = "12:34";
        al8[KEY_URI] = "";
        al8[KEY_ALARM_PERIOD] = "once";
        al8[KEY_ENABLED] = true;
        al8[KEY_ID] = "BadUID-Throws";
        alarms.append(al8);

        root[KEY_GROUP_ALARMS] = alarms;
    }

    void add_weather_config(QJsonObject& root) {
        /* Store Weather information*/
        QJsonObject json_weather;
        json_weather[KEY_WEATHER_API_KEY] =
            QString("d77bd1ca2fd77ce4e1cdcdd5f8b7206c");
        json_weather[KEY_WEATHER_LOCATION_ID] = QString("3452925");
        root[KEY_WEATHER] = json_weather;
    }
};

/*****************************************************************************/

TEST_F(ConfigurationFixture, read_radio_streams_two_streams) {
    auto& v = config->get_stations();
    ASSERT_EQ(2, v.size());
}
/*****************************************************************************/

TEST_F(ConfigurationFixture, addRadioStation_no_write) {
    config->add_radio_station(
        std::make_shared<PlayableItem>("foo", QUrl("http://bar.baz")));
    config->add_radio_station(
        std::make_shared<PlayableItem>("ref", QUrl("http://gmx.net")));
    auto& v = config->get_stations();
    ASSERT_EQ(4, v.size());
}
/*****************************************************************************/

TEST_F(ConfigurationFixture, addRadioStation_write) {
    {
        config->add_radio_station(
            std::make_shared<PlayableItem>("foo", QUrl("http://bar.baz")));
        config->add_radio_station(
            std::make_shared<PlayableItem>("ref", QUrl("http://gmx.net")));
        /* should write file in destructor */
        config->store_current_config();
    }
    Configuration control(filename, TEST_FILE_PATH);
    control.update_configuration();
    auto& v = control.get_stations();
    ASSERT_EQ(4, v.size());

    auto stream = v[2];
    ASSERT_EQ(stream->get_display_name(), QString("foo"));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, add_podcast_source) {
    QSignalSpy spy(config.get(), SIGNAL(podcast_sources_changed()));
    ASSERT_TRUE(spy.isValid());
    auto ps = std::make_shared<PodcastSource>(
        QUrl("https://alternativlos.org/alternativlos.rss"));
    auto size_before = config->get_podcast_sources().size();
    config->add_podcast_source(ps);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(config->get_podcast_sources().size(), size_before + 1);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, get_podcast_source_throws) {
    EXPECT_THROW(
        config->get_podcast_source(QUuid::createUuid()), std::out_of_range);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, get_podcast_source_ok) {
    auto& v = config->get_podcast_sources();
    auto uid = v[0]->get_id();
    auto item = config->get_podcast_source(uid);
    ASSERT_EQ(item, v[0].get());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, delete_podcast_source) {
    auto& v = config->get_podcast_sources();
    auto size_before = v.size();
    auto uid = v[0]->get_id();
    config->delete_podcast_source(uid);
    ASSERT_EQ(config->get_podcast_sources().size(), size_before - 1);
    EXPECT_THROW(config->get_podcast_source(uid), std::out_of_range);
}
/*****************************************************************************/
TEST_F(ConfigurationFixture, delete_podcast_throws) {
    EXPECT_THROW(
        config->delete_podcast_source(QUuid::createUuid()), std::out_of_range);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, add_radio_station) {
    QSignalSpy spy(config.get(), SIGNAL(stations_changed()));
    ASSERT_TRUE(spy.isValid());
    auto radio = std::make_shared<PlayableItem>();
    auto size_before = config->get_stations().size();
    config->add_radio_station(radio);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(config->get_stations().size(), size_before + 1);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, get_radio_station_throws) {
    EXPECT_THROW(config->get_station(QUuid::createUuid()), std::out_of_range);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, get_radio_station_ok) {
    auto& v = config->get_stations();
    auto uid = v[0]->get_id();
    auto item = config->get_station(uid);
    ASSERT_EQ(item, v[0].get());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, delete_radio_station) {
    auto& v = config->get_stations();
    auto size_before = v.size();
    auto uid = v[0]->get_id();
    config->delete_radio_station(uid);
    ASSERT_EQ(config->get_stations().size(), size_before - 1);
    EXPECT_THROW(config->get_station(uid), std::out_of_range);
}
/*****************************************************************************/
TEST_F(ConfigurationFixture, delete_radio_throws) {
    EXPECT_THROW(
        config->delete_radio_station(QUuid::createUuid()), std::out_of_range);
}


/*****************************************************************************/
TEST_F(ConfigurationFixture, add_alarm) {
    QSignalSpy spy(config.get(), SIGNAL(alarms_changed()));
    ASSERT_TRUE(spy.isValid());
    auto alm = std::make_shared<Alarm>();
    auto size_before = config->get_alarms().size();
    config->add_alarm(alm);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(config->get_alarms().size(), size_before + 1);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, get_alarm_throws) {
    EXPECT_THROW(config->get_alarm(QUuid::createUuid()), std::out_of_range);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, get_alarm_ok) {
    auto& v = config->get_alarms();
    auto uid = v[0]->get_id();
    auto item = config->get_alarm(uid);
    ASSERT_EQ(item, v[0].get());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, delete_alarm) {
    auto& v = config->get_alarms();
    auto size_before = v.size();
    auto uid = v[0]->get_id();
    config->delete_alarm(uid);
    ASSERT_EQ(config->get_alarms().size(), size_before - 1);
    EXPECT_THROW(config->get_alarm(uid), std::out_of_range);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, delete_alarm_throws) {
    EXPECT_THROW(config->delete_alarm(QUuid::createUuid()), std::out_of_range);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, ChangeAlarmEmitsAlarmsChanged) {
    auto& v = config->get_alarms();
    QSignalSpy spy(config.get(), SIGNAL(alarms_changed()));
    v[0]->enable(false);
    ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, read_2podcasts) {
    auto& v = config->get_podcast_sources();
    ASSERT_EQ(2, v.size());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, deletePodcastByIndex) {
    auto& v = config->get_podcast_sources();
    ASSERT_EQ(2, v.size());
    config->remove_podcast_source_by_index(0);
    ASSERT_EQ(config->get_podcast_sources().size(), 1);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, read_PodcastUri) {
    auto ps = config->get_podcast_source_by_index(0);
    ASSERT_EQ(
        ps->get_url(), QString("https://alternativlos.org/alternativlos.rss"));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, podcastSource_incomplete) {
    auto& v = config->get_podcast_sources();
    ASSERT_EQ(v[0]->get_url(),
        QString("https://alternativlos.org/alternativlos.rss"));
    ASSERT_EQ(v[1]->get_description(), QString(""));
    ASSERT_EQ(v[1]->get_episodes_names().size(), 0);
}

/*****************************************************************************/
TEST(StringToPeriodEnum, mapping_good) {
    ASSERT_EQ(Alarm::Once, string_to_alarm_period(KEY_ALARM_ONCE));
    ASSERT_EQ(Alarm::Daily, string_to_alarm_period(KEY_ALARM_DAILY));
    ASSERT_EQ(Alarm::Weekend, string_to_alarm_period(KEY_ALARM_WEEKEND));
    ASSERT_EQ(Alarm::Workdays, string_to_alarm_period(KEY_ALARM_WORKDAYS));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, alarm_count) {
    auto& v = config->get_alarms();
    // Alarm 5 has an unknown peridicity string "Manchmal"
    // Alarm 6 has an invalid Timestamp string "25:34"
    // Alarm 7 has an invalid URL
    ASSERT_EQ(v.size(), 4);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, alarm_id) {
    auto& v = config->get_alarms();
    auto res = std::find_if(
        v.begin(), v.end(), [&](const std::shared_ptr<Alarm>& item) {
            return item->get_id() ==
                QUuid("12eb4390-6abf-4626-be48-f11fe20f45cf");
        });
    ASSERT_NE(res, v.end());
    ASSERT_EQ((*res)->get_period(), Alarm::Workdays);
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, podcastid) {
    auto& v = config->get_podcast_sources();
    auto res = std::find_if(
        v.begin(), v.end(), [&](const std::shared_ptr<PodcastSource>& item) {
            return item->get_id() ==
                QUuid("a754affb-fd4b-4825-9eba-32b64fd7d50c");
        });
    ASSERT_NE(res, v.end());
    ASSERT_EQ((*res)->get_url(),
        QString("http://www.deutschlandfunk.de/"
                "podcast-essay-und-diskurs.1185.de.podcast.xml"));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, streamsourceid) {
    auto& v = config->get_stations();
    auto res = std::find_if(
        v.begin(), v.end(), [&](const std::shared_ptr<PlayableItem>& item) {
            return item->get_id() ==
                QUuid("c98a9c37-f990-47eb-ab73-21daf48d43c0");
        });
    ASSERT_NE(res, v.end());
    ASSERT_EQ((*res)->get_url(), QString("http://swr2.de"));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, alarm_daily) {
    auto& v = config->get_alarms();
    ASSERT_EQ(v[0]->get_period(), Alarm::Daily);
    ASSERT_EQ(v[0]->get_time(), QTime::fromString("10:00", "hh:mm"));
    ASSERT_TRUE(v[0]->is_enabled());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, alarm_workdays) {
    auto& v = config->get_alarms();

    ASSERT_EQ(v[1]->get_period(), Alarm::Workdays);
    ASSERT_EQ(v[1]->get_time(), QTime::fromString("07:00", "hh:mm"));
    ASSERT_TRUE(v[1]->is_enabled());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, alarm_weekends) {
    auto& v = config->get_alarms();
    ASSERT_EQ(v[2]->get_period(), Alarm::Weekend);
    ASSERT_EQ(v[2]->get_time(), QTime::fromString("09:00", "hh:mm"));
    ASSERT_FALSE(v[2]->is_enabled());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, alarm_once) {
    auto& v = config->get_alarms();
    ASSERT_EQ(v[3]->get_period(), Alarm::Once);
    ASSERT_EQ(v[3]->get_time(), QTime::fromString("13:00", "hh:mm"));
    ASSERT_TRUE(v[3]->is_enabled());
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, emitConfigChanged) {
    auto number_of_alarms = config->get_alarms().size();
    QSignalSpy spy(config.get(), SIGNAL(configuration_changed()));
    ASSERT_TRUE(spy.isValid());
    config->update_configuration();
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(config->get_alarms().size(), number_of_alarms);
}

/*****************************************************************************/
TEST(Configuration, DefaultForNotWritableCache) {
    QDir default_cache_dir(DEFAULT_CACHE_DIR_PATH);
    default_cache_dir.removeRecursively();
    ASSERT_FALSE(default_cache_dir.exists());

    Configuration config(DEFAULT_CONFIG_FILE_PATH, QString("/dev/"));

    ASSERT_TRUE(default_cache_dir.exists());
}

/*****************************************************************************/
TEST(Configuration, DefaultForNotWritableConfig) {
    QFile default_conf_file(DEFAULT_CONFIG_FILE_PATH);
    // Delete it should it exist..
    default_conf_file.remove();
    Configuration config(QString("/dev/foobar.json"), DEFAULT_CACHE_DIR_PATH);
    ASSERT_TRUE(default_conf_file.exists());
}

/*****************************************************************************/
TEST(Configuration, DefaultForNotReadableConfig) {
    QFile default_conf_file(DEFAULT_CONFIG_FILE_PATH);
    // Delete it should it exist..
    default_conf_file.remove();
    Configuration config(QString("/dev/mem"), DEFAULT_CACHE_DIR_PATH);
    ASSERT_TRUE(default_conf_file.exists());
}

/*****************************************************************************/
TEST(Configuration, ExceptionNotReadableConfig) {
    QFile cfg(QDir(QDir(DigitalRooster::TEST_FILE_PATH).filePath("testconfig"))
            .filePath("config-ro.json"));
    Configuration config(cfg.fileName(), DEFAULT_CACHE_DIR_PATH);
    cfg.setPermissions(QFileDevice::WriteOwner);
    ASSERT_THROW(config.update_configuration(), std::system_error);
    cfg.remove();
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, GetweatherConfigApiToken) {
    auto cfg = config->get_weather_config();
    ASSERT_EQ(cfg.get_api_token(), QString("d77bd1ca2fd77ce4e1cdcdd5f8b7206c"));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, GetweatherConfigCityId) {
    auto cfg = config->get_weather_config();
    ASSERT_EQ(cfg.get_location_id(), QString("3452925"));
}

/*****************************************************************************/
TEST_F(ConfigurationFixture, changeSleepTimeoutMinutes) {
    auto new_to = std::chrono::minutes(5);
    config->set_sleep_timeout(new_to);
    ASSERT_EQ(config->get_sleep_timeout(), new_to);
}

/*****************************************************************************/
