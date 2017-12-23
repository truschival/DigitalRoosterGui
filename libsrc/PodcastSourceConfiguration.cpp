/*************************************************************************************
 * \filename PodcastSourceConfiguration.cpp
 * \brief Implementation of the Class PodcastSourceConfiguration
 *
 * \details
 *
 * \author  ruschi 
 *************************************************************************************/


#include "PodcastSourceConfiguration.hpp"

using namespace DigitalRooster;


/*************************************************************************************/
void PodcastSourceConfiguration::add_episode(std::shared_ptr<PodcastEpisode> episode){
	episodes[episode->get_display_name()] = episode;
}


/*************************************************************************************/
void PodcastSourceConfiguration::remove_episode(const QString& name){


}
