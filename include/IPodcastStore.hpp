/******************************************************************************
 * \filename
 * \brief	Interface for creating/update and deletion Internet radio stations
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef INCLUDE_IPODSCASTSTORE_HPP_
#define INCLUDE_IPODSCASTSTORE_HPP_

#include <QUuid>
#include <memory>

namespace DigitalRooster {
class PodcastSource;

/**
 * Interface for addition, access and deletion of Podcast sources of
 * actual storage \ref DigitalRooster::ConfigurationManager
 */
class IPodcastStore {
public:
    /**
     * Append new PodcastSource to list
     * @param podcast source
     */
    virtual void add_podcast_source(std::shared_ptr<PodcastSource> podcast) = 0;

    /**
     * Delete a podcast source identified by id form the list of sources
     * @param id unique id of podcast source
     * @throws 	 std::out_of_range if not found
     */
    virtual void delete_podcast_source(const QUuid& id) = 0;
    /**
     * Get a single podcast source identified by ID
     * @throws 	 std::out_of_range if not found
     * @param id unique ID of podcast
     * @return source
     */
    virtual const PodcastSource* get_podcast_source(const QUuid& id) const = 0;

    /**
     * get all podcast sources
     */
    virtual const QVector<std::shared_ptr<PodcastSource>>&
    get_podcast_sources() const = 0;

    /**
     * Get a single podcast source identified by index
     * @throws 	 std::out_of_range if not found
     * @param index in vector
     * @return PodastSource
     */
    virtual PodcastSource* get_podcast_source_by_index(int index) const = 0;

    /**
     * Removes a podcast source entry form list
     * @throws 	 std::out_of_range if not found
     * @param index in vector
     */
    virtual void remove_podcast_source_by_index(int index) = 0;

    /**
     * virtual destructor
     */
    virtual ~IPodcastStore(){};
};


} // namespace DigitalRooster

#endif /* INCLUDE_IPODSCASTSTORE_HPP_ */
