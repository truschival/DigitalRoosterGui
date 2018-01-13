/*************************************************************************************
 * \filename PodcastSourceConfiguration.cpp
 * \brief Implementation of the Class PodcastSourceConfiguration
 *
 * \details
 *
 * \author  ruschi 
 *************************************************************************************/

#include <QDebug>
#include "PodcastSource.hpp"

using namespace DigitalRooster;


/*************************************************************************************/
void PodcastSource::add_episode(std::shared_ptr<PodcastEpisode> episode){
	if(episodes.size() < max_episodes){
		episodes[episode->get_display_name()] = episode;
	}
}


/*************************************************************************************/
void PodcastSource::remove_episode(const QString& name){
	episodes.remove(name);
}

