/******************************************************************************
 * \filename
 * \brief     Dispatches REST API calls to respective API implementations
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef INCLUDE_REST_RESTADAPTER_HPP_
#define INCLUDE_REST_RESTADAPTER_HPP_

#include <memory>
#include <string>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "ITimeoutStore.hpp"
#include "IWeatherConfigStore.hpp"

#include "AlarmApi.hpp"
#include "IAlarmStore.hpp"

#include "IPodcastStore.hpp"
#include "PodcastApi.hpp"

#include "IStationStore.hpp"
#include "RadioApi.hpp"

namespace DigitalRooster {
namespace REST {
    /**
     * Class to setup Pistache infrastructure and API implementations
     */
    class ApiHandler {
    public:
        ApiHandler(DigitalRooster::IWeatherConfigStore& ws,
            DigitalRooster::IAlarmStore& as, DigitalRooster::IPodcastStore& ps,
            DigitalRooster::IStationStore& sts,
            DigitalRooster::ITimeOutStore& tos, Pistache::Address addr);

        /**
         * Read list of podcast sources
         * @param request
         * @param response
         */
        void podcasts_read_list_handler(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Read list of alarms
         * @param request
         * @param response
         */
        void alarms_read_list_handler(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Catch all HTTP handler for unknown methods and resources
         * @param request
         * @param response
         */
        void default_handler(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        ~ApiHandler() = default;

    private:
        Pistache::Http::Endpoint endpoint;
        Pistache::Rest::Router router;
        REST::AlarmApi alarmapi;
        REST::RadioApi radioapi;
        REST::PodcastApi podcastsapi;
    };
} // namespace REST
} // namespace DigitalRooster
#endif /* INCLUDE_REST_RESTADAPTER_HPP_ */
