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
}
;

void ConfigurationManager::read_radio_streams_from_file() {
	QString key(KEY_GROUP_SOURCES.c_str());
	key.append("/").append(KEY_GROUP_IRADIO_SOURCES.c_str());
	int size = qs.beginReadArray(key);
	for (int i = 0; i < size; i++) {
		qs.setArrayIndex(i);

		stream_sources.emplace_back(
				qs.value(KEY_NAME.c_str(), "UNKNOWN_NAME").toString(),
				qs.value(KEY_URL.c_str(), "UNKNOWN_URL").toString());
	}
	qs.endArray();
}

/***********************************************************************************/
void ConfigurationManager::add_radio_station(RadioStreamSource&& src) {
	this->stream_sources.emplace_back(std::forward<RadioStreamSource>(src));
}

/***********************************************************************************/
void ConfigurationManager::add_radio_station(const RadioStreamSource& src) {
	this->stream_sources.push_back(src);
}

/***********************************************************************************/
void ConfigurationManager::write_config_file() {
	qs.beginGroup(KEY_GROUP_SOURCES.c_str());
	qs.beginWriteArray(KEY_GROUP_IRADIO_SOURCES.c_str(),stream_sources.size());
	int i =0;
	for(auto& e : stream_sources){
		qs.setArrayIndex(i++);
		qs.setValue(KEY_NAME.c_str(),e.get_display_name());
		qs.setValue(KEY_URL.c_str(),e.get_url());
	}
	qs.endArray();
	qs.endGroup();

	qs.sync();
}
