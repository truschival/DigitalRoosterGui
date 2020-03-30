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

#include "Helpers.h"
#include "PodcastSource.hpp"
#include "RestAdapter.hpp"

using namespace Pistache;
using namespace Pistache::Rest;
using namespace DigitalRooster;

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
        Routes::bind(&RestAdapter::podcasts_read_all_handler, this));

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
void RestAdapter::podcasts_read_all_handler(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    using namespace org::openapitools::server::helpers;

    // Offset default 0
    auto offset =
        get_uint_from_query_with_default(request.query().get("offset"), 0);
    // length default max possible length
    auto length =
        get_uint_from_query_with_default(request.query().get("length"), 1024);
    try {
        QJsonArray j;
        for (const auto& p : container_from_range(
                 podcaststore.get_podcast_sources(), offset, length)) {
            j.push_back(QJsonValue(p->to_json_object()));
        }
        QJsonDocument jdoc;
        jdoc.setArray(j);
        response.send(Pistache::Http::Code::Ok, jdoc.toJson().toStdString());
    } catch (std::exception& e) {
        // send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }
}

/*****************************************************************************/
unsigned long DigitalRooster::get_uint_from_query_with_default(
    const Pistache::Optional<std::string>& query, unsigned long def_val) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    int val = def_val;
    try {
        val = std::stoi(query.get());
    } catch (const std::invalid_argument& e) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << e.what();
    }
    // return only positive values, all others will default
    return (val < 0) ? def_val : val;
}
