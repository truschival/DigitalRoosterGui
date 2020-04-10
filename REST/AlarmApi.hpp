/******************************************************************************
 * \filename
 * \brief     API for managing Internet Alarm Stations
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef REST_ALARMAPI_HPP_
#define REST_ALARMAPI_HPP_

#include <pistache/http.h>
#include <pistache/router.h>
#include <string>

#include "IAlarmStore.hpp"

namespace DigitalRooster {
namespace REST {

    class AlarmApi {
    public:
        /**
         * API registers all handlers with router
         * @param alarmstore backend that provides access to Alarm list
         * @param router
         */
        AlarmApi(IAlarmStore& station, Pistache::Rest::Router& router);

        /**
         * resource name under \ref{API_URL_BASE}
         * @return
         */
        const std::string& get_api_resource_name() const {
            return api_ressource;
        }

        /**
         * Read list of Alarms and write it out via HTTP response
         * @param request with optional length and offset parameters
         * @param response
         */
        void read_alarm_list(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Add the alarm from request to list of Alarms
         * @param request must contain a valid JSON in body
         * @param response
         */
        void add_alarm(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Get the alarm  identified by UUID to list of Alarms
         * @param request with UUID as parameter
         * @param response
         */
        void get_alarm(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Update Alarm identified by UUID with new values
         * @param request with UUID as parameter and Alarm JSON
         * @param response
         */
        void update_alarm(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        /**
         * Delete an alarm with UUID from list of Alarms
         * @param request with UUID of Alarm to delete
         * @param response
         */
        void delete_alarm(const Pistache::Rest::Request& request,
            Pistache::Http::ResponseWriter response);

        virtual ~AlarmApi() = default;

    private:
        /**
         * Backend handling internet Alarm stations in configuration
         */
        IAlarmStore& alarmstore;
        /**
         * API resource name
         */
        const std::string api_ressource{"alarms"};
    };
} /* namespace REST */
} /* namespace DigitalRooster */

#endif /* REST_ALARMAPI_HPP_ */
