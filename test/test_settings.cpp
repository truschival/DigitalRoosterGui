/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include <QSettings>
#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include "appconstants.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

class SettingsFixture : public virtual ::testing::Test {
public:
    SettingsFixture()
        : filename("testsettings.json") {
    }

    ~SettingsFixture() {
    }

    void SetUp() {
        add_podcast_sources(appconfig);
        add_internet_radio(appconfig);
        QJsonDocument doc(appconfig);
        QFile tf(filename);
        tf.open(QIODevice::ReadWrite | QIODevice::Text);
        tf.write(doc.toJson());
        tf.close();
    }

    void TearDown() {
        QFile file(filename);
        file.remove();
    }

protected:
    QString filename;
    QJsonObject appconfig;

    void add_podcast_sources(QJsonObject& root) {
        QJsonArray radiosources;
        QJsonObject dradio;
        dradio[KEY_NAME] = "DRadio";
        dradio[KEY_URI] = "http://dradio.de";
        QJsonObject swr2;
        swr2[KEY_NAME] = "SWR2";
        swr2[KEY_URI] = "http://swr2.de";
        radiosources.append(dradio);
        radiosources.append(swr2);
        root[KEY_GROUP_IRADIO_SOURCES] = radiosources;
    }

    void add_internet_radio(QJsonObject& root) {
        QJsonArray pcsources;
        QJsonObject pc1;
        pc1[KEY_NAME] = "Alternativlos";
        pc1[KEY_URI] = "https://alternativlos.org/alternativlos.rss";
        QJsonObject pc2;
        pc2[KEY_NAME] = "Dradio Essay&Diskurs";
        pc2[KEY_URI] = "http://www.deutschlandfunk.de/"
                       "podcast-essay-und-diskurs.1185.de.podcast.xml";
        pcsources.append(pc1);
        pcsources.append(pc2);
        root[KEY_GROUP_PODCAST_SOURCES] = pcsources;
    }
};

TEST_F(SettingsFixture, read_radio_streams_two_streams) {
    ConfigurationManager cm(filename);
    auto& v = cm.get_stream_sources();
    ASSERT_EQ(2, v.size());
}

TEST_F(SettingsFixture, addRadioStation_no_write) {
    ConfigurationManager cm(filename);
    cm.add_radio_station(
        std::make_unique<RadioStream>("foo", QUrl("http://bar.baz")));
    cm.add_radio_station(
        std::make_unique<RadioStream>("ref", QUrl("http://gmx.net")));
    auto& v = cm.get_stream_sources();
    ASSERT_EQ(4, v.size());
}

TEST_F(SettingsFixture, addRadioStation_write) {
    {
        ConfigurationManager cm(filename);
        cm.add_radio_station(
            std::make_unique<RadioStream>("foo", QUrl("http://bar.baz")));
        cm.add_radio_station(
            std::make_unique<RadioStream>("ref", QUrl("http://gmx.net")));
        /* should write file in destructor */
        cm.write_config_file();
    }
    ConfigurationManager control(filename);
    auto& v = control.get_stream_sources();
    ASSERT_EQ(4, v.size());

    auto stream = v[2];
    ASSERT_EQ(stream->get_display_name(), QString("foo"));
}

TEST_F(SettingsFixture, read_2podcasts) {
    ConfigurationManager cm(filename);
    auto& v = cm.get_podcast_sources();
    ASSERT_EQ(2, v.size());
}

TEST_F(SettingsFixture, read_PodcastUri) {
    ConfigurationManager cm(filename);
    auto& v = cm.get_podcast_sources();
    ASSERT_EQ(v[0]->get_url(),
        QString("https://alternativlos.org/alternativlos.rss"));
    ASSERT_EQ(v[1]->get_url(),
        QString("http://www.deutschlandfunk.de/"
                "podcast-essay-und-diskurs.1185.de.podcast.xml"));
}

TEST_F(SettingsFixture, podcastSource_incomplete) {
    ConfigurationManager cm(filename);
    auto& v = cm.get_podcast_sources();
    ASSERT_EQ(v[0]->get_url(),
        QString("https://alternativlos.org/alternativlos.rss"));
    ASSERT_EQ(v[1]->get_description(), QString(""));
    ASSERT_EQ(v[1]->get_episodes_names().size(), 0);
}
