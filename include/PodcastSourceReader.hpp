/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/


#ifndef _PODCASTSOURCEREADER_HPP_
#define _PODCASTSOURCEREADER_HPP_ 


#include "PodcastSource.hpp"

namespace DigitalRooster {

/**
 * retrieve and parse RSS feed for general information, stores the retrieved RSS
 * in the PodcastSource configuration
 * @param podcastsource podcast source
 */
void update_podcast(PodcastSource& podcastsource);

}
;
#endif // _PODCASTSOURCEREADER_HPP_ 
