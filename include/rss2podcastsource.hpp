/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _RSS2PODCASTSOURCE_HPP_
#define _RSS2PODCASTSOURCE_HPP_

#include "PodcastSource.hpp"

namespace DigitalRooster {

/**
 * parse RSS feed and update podcastsource
 *
 * @param podcastsource podcast source
 * @param data - RSS XML feed from network reply or file or else
 */
void update_podcast(PodcastSource& podcastsource, const QByteArray& data);

};     // namespace DigitalRooster

#endif /* _RSS2PODCASTSOURCE_HPP_ */
