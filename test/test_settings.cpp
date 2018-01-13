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
#include <QSettings>

#include <string>
#include <fstream>
#include <cstdio>

#include "configuration_manager.hpp"

using namespace DigitalRooster;

class SettingsFixture: public virtual ::testing::Test {
public:
	SettingsFixture() :
			filename("testsettings.ini"), qs(filename.c_str(),
					QSettings::IniFormat) {

	}

	~SettingsFixture() {

	}

	void SetUp() {
		qs.beginGroup(DigitalRooster::KEY_GROUP_SOURCES.c_str());
		qs.beginWriteArray(DigitalRooster::KEY_GROUP_PODCAST_SOURCES.c_str(),
				2);
		qs.setArrayIndex(0);
		qs.setValue(DigitalRooster::KEY_URL.c_str(),
				"https://alternativlos.org/alternativlos.rss");
		qs.setArrayIndex(1);
		qs.setValue(DigitalRooster::KEY_URL.c_str(),
				"http://www.deutschlandfunk.de/podcast-essay-und-diskurs.1185.de.podcast.xml");

		qs.endArray(); // Internet radio sources
		qs.endGroup();
		qs.sync();
	}

	void TearDown() {
		std::remove(filename.c_str());
	}

protected:
	std::string filename;
	QSettings qs;

};

TEST_F(SettingsFixture,read_radio_streams_file_empty) {
	ConfigurationManager cm(filename);
	auto& v = cm.get_stream_sources();
	ASSERT_EQ(0, v.size());
}

TEST_F(SettingsFixture,read_radio_streams_one_stream) {
	QSettings control(filename.c_str(), QSettings::NativeFormat);
	control.beginGroup(DigitalRooster::KEY_GROUP_SOURCES.c_str());
	control.beginWriteArray(DigitalRooster::KEY_GROUP_IRADIO_SOURCES.c_str(),
			1);
	control.beginGroup("SWR2");
	control.setValue(DigitalRooster::KEY_NAME.c_str(), "SWR-2");
	control.setValue(DigitalRooster::KEY_URL.c_str(), "http://swr2.de");
	control.endGroup();
	control.endArray(); // Internet radio sources
	control.endGroup();
	control.sync();

	ConfigurationManager cm(filename);
	auto& v = cm.get_stream_sources();
	ASSERT_EQ(1, v.size());
}

TEST_F(SettingsFixture,read_radio_streams_two_streams) {
	QSettings control(filename.c_str(), QSettings::NativeFormat);
	control.beginGroup(DigitalRooster::KEY_GROUP_SOURCES.c_str());
	control.beginWriteArray(DigitalRooster::KEY_GROUP_IRADIO_SOURCES.c_str(),
			2);
	control.beginGroup("SWR2");
	control.setValue(DigitalRooster::KEY_NAME.c_str(), "SWR-2");
	control.setValue(DigitalRooster::KEY_URL.c_str(), "http://swr2.de");
	control.endGroup();

	control.beginGroup("DRADIO");
	control.setValue(DigitalRooster::KEY_NAME.c_str(), "dradio1");
	control.setValue(DigitalRooster::KEY_URL.c_str(), "http://dradio.de");
	control.endGroup();

	control.endArray(); // Internet radio sources
	control.endGroup();
	control.sync();

	ConfigurationManager cm(filename);
	auto& v = cm.get_stream_sources();
	ASSERT_EQ(2, v.size());
}

TEST_F(SettingsFixture,addRadioStation_no_write) {
	ConfigurationManager cm(filename);
	cm.add_radio_station(
			std::make_unique<RadioStream>("foo", QUrl("http://bar.baz")));
	cm.add_radio_station(
			std::make_unique<RadioStream>("ref", QUrl("http://gmx.net")));
	auto& v = cm.get_stream_sources();
	ASSERT_EQ(2, v.size());
}

TEST_F(SettingsFixture,addRadioStation_write) {
	{
		ConfigurationManager cm(filename);
		cm.add_radio_station(
				std::make_unique<RadioStream>("foo", QUrl("http://bar.baz")));
		cm.add_radio_station(
				std::make_unique<RadioStream>("ref", QUrl("http://gmx.net")));
		auto& v = cm.get_stream_sources();
		ASSERT_EQ(2, v.size());
		/* should write file in destructor */
		cm.write_config_file();
	}
	ConfigurationManager control(filename);
	auto& v = control.get_stream_sources();
	ASSERT_EQ(2, v.size());

	auto stream = v[0];
	ASSERT_EQ(stream->get_display_name(), QString("foo"));
}

TEST_F(SettingsFixture,read_2podcasts) {
	ConfigurationManager cm(filename);
	auto& v = cm.get_podcast_sources();
	ASSERT_EQ(2, v.size());
}

TEST_F(SettingsFixture,read_PodcastUri) {
	ConfigurationManager cm(filename);
	auto& v = cm.get_podcast_sources();
	ASSERT_EQ(v[0]->get_url(),
			QString("https://alternativlos.org/alternativlos.rss"));
	ASSERT_EQ(v[1]->get_url(),
			QString(
					"http://www.deutschlandfunk.de/podcast-essay-und-diskurs.1185.de.podcast.xml"));
}

TEST_F(SettingsFixture,podcastSource_incomplete) {
	ConfigurationManager cm(filename);
	auto& v = cm.get_podcast_sources();
	ASSERT_EQ(v[0]->get_url(),
			QString("https://alternativlos.org/alternativlos.rss"));
	ASSERT_EQ(v[1]->get_description(),QString(""));
	ASSERT_EQ(v[1]->get_episodes_names().size(),0);
}

