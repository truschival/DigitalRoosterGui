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
	int size = qs.beginReadArray("Sources/InternetRadio");
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
