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
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> newep) {
    if (episodes.size() < max_episodes) {
        auto ep = std::find_if(episodes.begin(), episodes.end(),
            [newep](std::shared_ptr<PodcastEpisode> episode) {
                // qDebug() << "Episode guid " << episode->get_guid() << "newep : "
                // <<newep->get_guid();
                return episode->get_guid() == newep->get_guid();
            });

        if (ep == episodes.end()) {
            episodes.push_back(newep);
        }
    }
}


/*************************************************************************************/
void PodcastSource::newFileAvailable(const QString& filename) {
    emit dataChanged();
}

/*************************************************************************************/
QVector<QString> PodcastSource::get_episodes_names() {
    auto ret = QVector<QString>();
    for (const auto& e : episodes) {
        ret.push_back(e->get_display_name());
    }
    return ret;
}
