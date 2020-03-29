/******************************************************************************
 * \filename
 * \brief     REST adapter for configuration
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef INCLUDE_REST_RESTADAPTER_HPP_
#define INCLUDE_REST_RESTADAPTER_HPP_

#include <memory>
#include <string>

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "configuration_manager.hpp"

namespace DigitalRooster {
class RestAdapter {
public:
    const std::string base = "/v1";

    RestAdapter(const DigitalRooster::IWeatherConfigStore& ws,
        const DigitalRooster::IAlarmStore& asr,
        const DigitalRooster::IPodcastStore& ps,
        const DigitalRooster::IStationStore& sts,
        const DigitalRooster::ITimeOutStore& tos,
        Pistache::Address addr);

    void podcasts_read_all_handler(const Pistache::Rest::Request& request,
        Pistache::Http::ResponseWriter response);

private:
    void setup_routes();
    const DigitalRooster::IAlarmStore& alarmstore;
    const DigitalRooster::IPodcastStore& podcaststore;
    const DigitalRooster::IStationStore& stationstore;
    const DigitalRooster::ITimeOutStore& timeoutstore;

    Pistache::Http::Endpoint endpoint;
    Pistache::Rest::Router router;
};

} // namespace DigitalRooster
#endif /* INCLUDE_REST_RESTADAPTER_HPP_ */
