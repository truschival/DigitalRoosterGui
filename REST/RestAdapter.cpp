/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QLoggingCategory>

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>

#include <pistache/router.h>

#include "RestAdapter.hpp"
#include "PodcastSource.hpp"
#include "Helpers.h"

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
    const DigitalRooster::ITimeOutStore& tos,
    Pistache::Address addr)
    : alarmstore(asr)
    , podcaststore(ps)
    , stationstore(sts)
    , timeoutstore(tos)
    , endpoint(addr ) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto opts =
        Pistache::Http::Endpoint::options().threads(PISTACHE_SERVER_THREADS);
    opts.flags(Pistache::Tcp::Options::ReuseAddr);
    opts.maxRequestSize(PISTACHE_SERVER_MAX_REQUEST_SIZE);
    opts.maxResponseSize(PISTACHE_SERVER_MAX_RESPONSE_SIZE);
    endpoint.init(opts);

    Routes::Get(router, base + "/podcasts",
        Routes::bind(&RestAdapter::podcasts_read_all_handler, this));

};
/*****************************************************************************/

void RestAdapter::podcasts_read_all_handler(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    using namespace org::openapitools::server::helpers;

    // Getting the query params
    auto lengthQuery = request.query().get("length");
    Pistache::Optional<int32_t> length;
    if (!lengthQuery.isEmpty()) {
        int32_t value;
        if (fromStringValue(lengthQuery.get(), value)) {
            length = Pistache::Some(value);
        }
    }
    auto offsetQuery = request.query().get("offset");
    Pistache::Optional<int32_t> offset;
    if (!offsetQuery.isEmpty()) {
        int32_t value;
        if (fromStringValue(offsetQuery.get(), value)) {
            offset = Pistache::Some(value);
        }
    }

    try {
        QJsonArray j;
        for(const auto& pc : podcaststore.get_podcast_sources()){
        	j.push_back(QJsonValue(pc->to_json_object()));
        }
        QJsonDocument jdoc;
        jdoc.setArray(j);

		response.send(Pistache::Http::Code::Ok,jdoc.toJson().toStdString());
    } catch (std::exception& e) {
        // send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }
}
