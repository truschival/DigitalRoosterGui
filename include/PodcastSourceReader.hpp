/*************************************************************************************
 * \filename    
 * \brief
 *
 * \details
 * 
 * \author ruschi
 * 
 *
 *************************************************************************************/

#ifndef _PODCASTSOURCEREADER_HPP_
#define _PODCASTSOURCEREADER_HPP_ 


#include "PodcastSource.hpp"

namespace DigitalRooster {

/**
 * retrieve and parse RSS feed for general information, stores the retrieved RSS
 * in the PodcastSourceConfiguration
 */
extern "C" void update_podcast(PodcastSource* podcastsource);

}
;
#endif // _PODCASTSOURCEREADER_HPP_ 
