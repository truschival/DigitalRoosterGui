/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include <configuration_manager.hpp>

using namespace DigitalRooster;

ConfigurationManager::ConfigurationManager(const std::string& filepath) :
		qs(filepath.c_str(), QSettings::IniFormat) {
	read_radio_streams_from_file();
	read_podcasts_from_file();
}
;

/***********************************************************************************/
void ConfigurationManager::read_radio_streams_from_file() {
	QString key(KEY_GROUP_SOURCES.c_str());
	key.append("/").append(KEY_GROUP_IRADIO_SOURCES.c_str());
	int size = qs.beginReadArray(key);
	for (int i = 0; i < size; i++) {
		qs.setArrayIndex(i);

		stream_sources.push_back(
				std::make_shared<RadioStream>(
						qs.value(KEY_NAME.c_str(), "UNKNOWN_NAME").toString(),
						qs.value(KEY_URL.c_str(), "UNKNOWN_URL").toString()));
	}
	qs.endArray();
}

/***********************************************************************************/
void ConfigurationManager::read_podcasts_from_file() {
	QString key(KEY_GROUP_SOURCES.c_str());
	key.append("/").append(KEY_GROUP_PODCAST_SOURCES.c_str());
	int size = qs.beginReadArray(key);
	for (int i = 0; i < size; i++) {
		qs.setArrayIndex(i);
		QUrl url(qs.value(KEY_URL.c_str(), "UNKNOWN_URL").toString());
		if (QUrl(url).isValid()) {
			podcast_sources.push_back(
					std::make_shared<PodcastSourceConfiguration>(url));
		}
	}
	qs.endArray();
}
/***********************************************************************************/
void ConfigurationManager::add_radio_station(
		std::unique_ptr<RadioStream> src) {
	this->stream_sources.push_back(
			std::shared_ptr<RadioStream>(std::move(src)));
}

/***********************************************************************************/
void ConfigurationManager::write_config_file() {
	qs.beginGroup(KEY_GROUP_SOURCES.c_str());
	qs.beginWriteArray(KEY_GROUP_IRADIO_SOURCES.c_str(), stream_sources.size());
	int i = 0;
	for (auto& e : stream_sources) {
		qs.setArrayIndex(i++);
		qs.setValue(KEY_NAME.c_str(), e->get_display_name());
		qs.setValue(KEY_URL.c_str(), e->get_url());
	}
	qs.endArray();
	qs.endGroup();

	qs.sync();
}
