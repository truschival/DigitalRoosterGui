/******************************************************************************
 * \filename
 * \brief     API for managing Podcast RSS feeds
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef REST_PodcastApi_HPP_
#define REST_PodcastApi_HPP_

#include <pistache/http.h>
#include <pistache/router.h>
#include <string>

#include "IPodcastStore.hpp"

namespace DigitalRooster {
namespace REST {

    class PodcastApi {
    public:
        /**
         * API registers all handlers with router
         * @param ps backend that provides access list of PodcastSource
         * @param router
         */
        PodcastApi(IPodcastStore& ps, Pistache::Rest::Router& router);

        /**
         * resource name under \ref{API_URL_BASE}
         * @return
         */
        const std::string& get_api_resource_name() const {
            return api_ressource;
        }

        /**
         * Read list of PodcastSource and write it out via HTTP response
         * @param request with length and offset
         * @param response
         */
        void read_podcast_list(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Add a PodcastSource from request to list
         * @param request must contain a valid JSON for PodcastSource
         * @param response
         */
        void add_podcast(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Get the PodcastSource identified by UUID to list of PodcastSources
         * @param request with UUID as parameter
         * @param response
         */
        void get_podcast(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Delete PodcastSource with UUID from list of PodcastSources
         * @param request with UUID of PodcastSource to delete
         * @param response
         */
        void delete_podcast(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

    private:
        /**
         * Backend handling for storing PodcastSource in configuration
         */
        IPodcastStore& podcaststore;
        /**
         * API resource name
         */
        const std::string api_ressource{"podcasts"};
    };
} /* namespace REST */
} /* namespace DigitalRooster */

#endif /* REST_PodcastApi_HPP_ */
