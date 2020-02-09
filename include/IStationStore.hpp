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
#ifndef INCLUDE_ISTATIONSTORE_HPP_
#define INCLUDE_ISTATIONSTORE_HPP_

#include <QUuid>
#include <memory>

namespace DigitalRooster {
class PlayableItem;

class IStationStore {
public:
    /**
     * Append the radio stream to list - duplicates will not be checked
     * @param src the new stream source - we take ownership
     */
    virtual void add_radio_station(std::shared_ptr<PlayableItem> src) = 0;
    /**
     * Delete a internet radio station identified by id form the list
     * @param id unique id of radio station
     * @throws 	 std::out_of_range if not found
     */
    virtual void delete_radio_station(const QUuid& id) = 0;

    /**
     * get all radio stream sources
     */
    virtual const QVector<std::shared_ptr<PlayableItem>>&
    get_stations() const = 0;

    /**
     * Get a internet radio station identified by ID
     * @throws 	 std::out_of_range if not found
     * @param id unique ID of podcast
     * @return station
     */
    virtual const PlayableItem* get_station(const QUuid& id) const = 0;

    /**
     * virtual destructor
     */
    virtual ~IStationStore(){};
};


} // namespace DigitalRooster

#endif /* INCLUDE_ISTATIONSTORE_HPP_ */
