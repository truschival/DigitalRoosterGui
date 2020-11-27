/******************************************************************************
 * \filename
 * \brief     Interface to REST API using pimpl
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef INCLUDE_RESTAPI_HPP_
#define INCLUDE_RESTAPI_HPP_

#include <memory>

#include "IAlarmStore.hpp"
#include "IPodcastStore.hpp"
#include "IStationStore.hpp"
#include "ITimeoutStore.hpp"
#include "IWeatherConfigStore.hpp"

namespace DigitalRooster {
namespace REST {
    class ApiHandler;
}

/**
 * Interface class around the REST Server
 * glues ConfigurationManager interfaces to REST API
 */
class RestApi {
public:
	/**
     * Constructor with all dependencies
     * @param ws weather storage provider
     * @param asr alarm storage provider
     * @param ps  podcast storage provider
     * @param sts station storage provider
     * @param tos timeout storage provider
     */
    RestApi(DigitalRooster::IWeatherConfigStore& ws,
        DigitalRooster::IAlarmStore& asr,
        DigitalRooster::IPodcastStore& ps,
        DigitalRooster::IStationStore& sts,
        DigitalRooster::ITimeOutStore& tos);

    ~RestApi();
    /* do not copy, move or assign */
    RestApi(const RestApi& rhs) = delete;
    RestApi(RestApi&& rhs) = delete;
    RestApi& operator=(RestApi&& rhs) = delete;
    RestApi& operator=(const RestApi& rhs) = delete;

private:
    /**
     * Real implementation
     */
    std::unique_ptr<DigitalRooster::REST::ApiHandler> impl;
};
} // namespace DigitalRooster
#endif /* INCLUDE_RESTAPI_HPP_ */
