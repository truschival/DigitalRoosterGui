/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#include "UpdateTask.hpp"
#include <QString>
#include <configuration_manager.hpp>

using namespace DigitalRooster;

ConfigurationManager::ConfigurationManager(const QString& filepath)
    : qs(filepath, QSettings::IniFormat) {
    read_radio_streams_from_file();
    read_podcasts_from_file();
};

/***********************************************************************************/
void ConfigurationManager::read_radio_streams_from_file() {
    QString key(KEY_GROUP_SOURCES);
    key.append("/").append(KEY_GROUP_IRADIO_SOURCES);
    int size = qs.beginReadArray(key);
    for (int i = 0; i < size; i++) {
        qs.setArrayIndex(i);

        stream_sources.push_back(std::make_shared<RadioStream>(
            qs.value(KEY_NAME, "UNKNOWN_NAME").toString(),
            qs.value(KEY_URL, "UNKNOWN_URL").toString()));
    }
    qs.endArray();
}

/***********************************************************************************/
void ConfigurationManager::read_podcasts_from_file() {
    QString key(KEY_GROUP_SOURCES);
    key.append("/").append(KEY_GROUP_PODCAST_SOURCES);
    int size = qs.beginReadArray(key);
    for (int i = 0; i < size; i++) {
        qs.setArrayIndex(i);
        QUrl url(qs.value(KEY_URL, "UNKNOWN_URL").toString());
        if (QUrl(url).isValid()) {
            auto ps = std::make_shared<PodcastSource>(url);
            podcast_sources.push_back(ps);
        }
    }
    qs.endArray();
}
/***********************************************************************************/
void ConfigurationManager::add_radio_station(std::unique_ptr<RadioStream> src) {
    this->stream_sources.push_back(std::shared_ptr<RadioStream>(std::move(src)));
}

/***********************************************************************************/
void ConfigurationManager::write_config_file() {
    qs.beginGroup(KEY_GROUP_SOURCES);
    qs.beginWriteArray(KEY_GROUP_IRADIO_SOURCES, stream_sources.size());
    int i = 0;
    for (auto& e : stream_sources) {
        qs.setArrayIndex(i++);
        qs.setValue(KEY_NAME, e->get_display_name());
        qs.setValue(KEY_URL, e->get_url());
    }
    qs.endArray();
    qs.endGroup();

    qs.sync();
}
