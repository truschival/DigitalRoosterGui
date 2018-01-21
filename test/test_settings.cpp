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
			filename("testsettings.ini"), qs(filename,
					QSettings::IniFormat) {

	}

	~SettingsFixture() {

	}

	void SetUp() {
		qs.beginGroup(DigitalRooster::KEY_GROUP_SOURCES);
		qs.beginWriteArray(DigitalRooster::KEY_GROUP_PODCAST_SOURCES,
				2);
		qs.setArrayIndex(0);
		qs.setValue(DigitalRooster::KEY_URL,
				"https://alternativlos.org/alternativlos.rss");
		qs.setArrayIndex(1);
		qs.setValue(DigitalRooster::KEY_URL,
				"http://www.deutschlandfunk.de/podcast-essay-und-diskurs.1185.de.podcast.xml");

		qs.endArray(); // Internet radio sources
		qs.endGroup();
		qs.sync();
	}

	void TearDown() {
		QFile file(filename);
		file.remove();
	}

protected:
	QString filename;
	QSettings qs;

};

TEST_F(SettingsFixture,read_radio_streams_file_empty) {
	ConfigurationManager cm(filename);
	auto& v = cm.get_stream_sources();
	ASSERT_EQ(0, v.size());
}

TEST_F(SettingsFixture,read_radio_streams_one_stream) {
	QSettings control(filename, QSettings::NativeFormat);
	control.beginGroup(DigitalRooster::KEY_GROUP_SOURCES);
	control.beginWriteArray(DigitalRooster::KEY_GROUP_IRADIO_SOURCES,
			1);
	control.beginGroup("SWR2");
	control.setValue(DigitalRooster::KEY_NAME, "SWR-2");
	control.setValue(DigitalRooster::KEY_URL, "http://swr2.de");
	control.endGroup();
	control.endArray(); // Internet radio sources
	control.endGroup();
	control.sync();

	ConfigurationManager cm(filename);
	auto& v = cm.get_stream_sources();
	ASSERT_EQ(1, v.size());
}

TEST_F(SettingsFixture,read_radio_streams_two_streams) {
	QSettings control(filename, QSettings::NativeFormat);
	control.beginGroup(DigitalRooster::KEY_GROUP_SOURCES);
	control.beginWriteArray(DigitalRooster::KEY_GROUP_IRADIO_SOURCES,
			2);
	control.beginGroup("SWR2");
	control.setValue(DigitalRooster::KEY_NAME, "SWR-2");
	control.setValue(DigitalRooster::KEY_URL, "http://swr2.de");
	control.endGroup();

	control.beginGroup("DRADIO");
	control.setValue(DigitalRooster::KEY_NAME, "dradio1");
	control.setValue(DigitalRooster::KEY_URL, "http://dradio.de");
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

