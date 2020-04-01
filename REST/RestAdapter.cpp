/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QLoggingCategory>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <pistache/router.h>

#include "PodcastSource.hpp"
#include "alarm.hpp"

#include "RestAdapter.hpp"
#include "common.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.RestAdapter");

/*****************************************************************************/

const int PISTACHE_SERVER_THREADS = 2;
const int PISTACHE_SERVER_MAX_REQUEST_SIZE = 32768;
const int PISTACHE_SERVER_MAX_RESPONSE_SIZE = 32768;

/*****************************************************************************/
RestAdapter::RestAdapter(const DigitalRooster::IWeatherConfigStore& ws,
    const DigitalRooster::IAlarmStore& asr,
    const DigitalRooster::IPodcastStore& ps,
    const DigitalRooster::IStationStore& sts,
    const DigitalRooster::ITimeOutStore& tos, Pistache::Address addr)
    : alarmstore(asr)
    , podcaststore(ps)
    , stationstore(sts)
    , timeoutstore(tos)
    , endpoint(addr) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto opts =
        Pistache::Http::Endpoint::options().threads(PISTACHE_SERVER_THREADS);
    opts.flags(Pistache::Tcp::Options::ReuseAddr);
    opts.maxRequestSize(PISTACHE_SERVER_MAX_REQUEST_SIZE);
    opts.maxResponseSize(PISTACHE_SERVER_MAX_RESPONSE_SIZE);
    endpoint.init(opts);

    Routes::Get(router, base + "/podcasts",
        Routes::bind(&RestAdapter::podcasts_read_list_handler, this));

    Routes::Get(router, base + "/alarms",
        Routes::bind(&RestAdapter::alarms_read_list_handler, this));

    Routes::Get(router, base + "/radios",
            Routes::bind(&RestAdapter::radios_read_list_handler, this));

    router.addCustomHandler(Routes::bind(&RestAdapter::default_handler, this));
    endpoint.setHandler(router.handler());
    endpoint.serveThreaded();
};

/*****************************************************************************/
void RestAdapter::default_handler(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.send(Pistache::Http::Code::Not_Found,
        R"({
  "code": 404, 
  "message": "The resource or method does not exist!"
})");
}


/*****************************************************************************/
void RestAdapter::podcasts_read_list_handler(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(podcaststore.get_podcast_sources(), request, response);
}


/*****************************************************************************/
void RestAdapter::alarms_read_list_handler(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(alarmstore.get_alarms(), request, response);
}

/*****************************************************************************/
void RestAdapter::radios_read_list_handler(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(stationstore.get_stations(), request, response);
}


/*****************************************************************************/
int DigitalRooster::REST::get_val_from_query_within_range(
    const Pistache::Optional<std::string>& query, int min, int max) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    int val = min;
    try {
        val = std::stoi(query.get());
    } catch (const std::invalid_argument& e) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << e.what();
    }
    // cap val at lower end
    val = (val < min) ? min : val;
    // vap val at upper limit
    val = (val > max) ? max : val;
    return val;
}
