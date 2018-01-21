/*************************************************************************************
 * \filename PodcastSourceConfiguration.cpp
 * \brief Implementation of the Class PodcastSourceConfiguration
 *
 * \details
 *
 * \author  ruschi
 *************************************************************************************/

#include "PodcastSource.hpp"
#include <QDebug>
#include <algorithm>

using namespace DigitalRooster;


/*************************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> episode) {
    if (episodes.size() < max_episodes) {
        episodes.push_back(episode);
    }
}


/*************************************************************************************/
void PodcastSource::newFileAvailable(const QString& filename) {
}

/*************************************************************************************/
QVector<QString> PodcastSource::get_episodes_names() {
    auto ret = QVector<QString>(episodes.size());
    for (const auto& e : episodes) {
        ret.push_back(e->get_display_name());
    }
    return ret;
}

/*************************************************************************************/

void PodcastSource::updatedEpisodesAvailable(
    QVector<std::shared_ptr<DigitalRooster::PodcastEpisode>>) {

}
