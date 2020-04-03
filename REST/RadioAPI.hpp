/******************************************************************************
 * \filename
 * \brief     API for managing Internet Radio Stations
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef REST_RADIOAPI_HPP_
#define REST_RADIOAPI_HPP_

#include <pistache/router.h>

#include "IStationStore.hpp"

namespace DigitalRooster {
namespace REST {

    class RadioAPI {
    public:
        /**
         * API registers all handlers with router
         * @param station backend that provides access to radio stations
         * @param router
         */
        RadioAPI(IStationStore& station, Pistache::Rest::Router& router);

        /**
         * resource name under \ref{API_URL_BASE}
         * @return
         */
        const std::string& get_api_resource_name() const {
            return api_ressource;
        }

        /**
         * Read list of radio stations and write it out via HTTP response
         * @param request
         * @param response
         */
        void read_radio_list(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Add the station from request to list of radio stations
         * @param request must contain a valid JSON for Radio station
         * @param response
         */
        void add_station(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Get the station identified by UUID to list of radio stations
         * @param request with UUID as parameter
         * @param response
         */
        void get_station(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Update station identified by UUID with new values
         * @param request with UUID as parameter and station information body
         * @param response
         */
        void update_station(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Delete a station with UUID from list of radio stations
         * @param request with UUID of radio station to delete
         * @param response
         */
        void delete_station(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        virtual ~RadioAPI() = default;

    private:
        /**
         * Backend handling internet radio stations in configuration
         */
        IStationStore& stationstore;
        /**
         * API resource name
         */
        const std::string api_ressource{"radios/"};
    };
} /* namespace REST */
} /* namespace DigitalRooster */

#endif /* REST_RADIOAPI_HPP_ */
