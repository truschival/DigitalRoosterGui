/******************************************************************************
 * \filename
 * \brief     REST adapter for configuration
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

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "IAlarmStore.hpp"
#include "IPodcastStore.hpp"
#include "IStationStore.hpp"
#include "ITimeoutStore.hpp"
#include "IWeatherConfigStore.hpp"

namespace DigitalRooster {
class RestAdapter {
public:
    const std::string base = "/v1";

    RestAdapter(const DigitalRooster::IWeatherConfigStore& ws,
        const DigitalRooster::IAlarmStore& asr,
        const DigitalRooster::IPodcastStore& ps,
        const DigitalRooster::IStationStore& sts,
        const DigitalRooster::ITimeOutStore& tos, Pistache::Address addr);

    void podcasts_read_all_handler(const Pistache::Rest::Request& request,
        Pistache::Http::ResponseWriter response);

    /**
     * Catch all HTTP handler for unknown methods and resources
     * @param request
     * @param response
     */
    void default_handler(const Pistache::Rest::Request& request,
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

/**
 * Helper function to read a range with offset and length from T
 * and return this range as a copy
 * @param all a container with linear access iterators
 * @param offset start index
 * @param length max number of items to read (if available)
 * @return QJsonArray with items form T
 */
template <typename T>
T container_from_range(
    const T& all, unsigned long offset, unsigned long length) {
    auto start_it = all.begin() + offset;
    auto end_it = std::min(start_it + length, all.end());
    return T(start_it, end_it);
}
/**
 * Helper function to get an initialized size_t variable from optional query
 * parameters such as length or offset
 * @param query Pistache HTTP query that may or may not contain a numeric value
 * for variable
 * @param def_val initializer value for variable if query did not contain a
 * value
 */
unsigned long get_uint_from_query_with_default(
    const Pistache::Optional<std::string>& query, unsigned long def_val);

} // namespace DigitalRooster
#endif /* INCLUDE_REST_RESTADAPTER_HPP_ */
