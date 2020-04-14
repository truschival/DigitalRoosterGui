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
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLoggingCategory>

#include <memory>
#include <string>

#include <pistache/http.h>
#include <pistache/router.h>

#include "ApiHandler.hpp"
#include "RestApi.hpp"
#include "appconstants.hpp"
#include "common.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "ApiHandler");

/*****************************************************************************/

const int PISTACHE_SERVER_THREADS = 2;
const int PISTACHE_SERVER_MAX_REQUEST_SIZE = 32768;
const int PISTACHE_SERVER_MAX_RESPONSE_SIZE = 32768;

/*****************************************************************************/
/* PIMPL initialization */
DigitalRooster::RestApi::RestApi(DigitalRooster::IWeatherConfigStore& ws,
    DigitalRooster::IAlarmStore& asr, DigitalRooster::IPodcastStore& ps,
    DigitalRooster::IStationStore& sts, DigitalRooster::ITimeOutStore& tos)
    : impl(std::make_unique<ApiHandler>(ws, asr, ps, sts, tos,
          Pistache::Address(
              Pistache::Ipv4::any(), Pistache::Port(REST_API_PORT)))) {
}
/*****************************************************************************/
DigitalRooster::RestApi::~RestApi() {
    // default dtor for PIMPL
}

/*****************************************************************************/
ApiHandler::ApiHandler(DigitalRooster::IWeatherConfigStore& ws,
    DigitalRooster::IAlarmStore& as, DigitalRooster::IPodcastStore& ps,
    DigitalRooster::IStationStore& sts, DigitalRooster::ITimeOutStore& tos,
    Pistache::Address addr)
    : endpoint(addr)
    , alarmapi(as, router)
    , radioapi(sts, router)
    , podcastsapi(ps, router) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto opts =
        Pistache::Http::Endpoint::options().threads(PISTACHE_SERVER_THREADS);
    opts.flags(Pistache::Tcp::Options::ReuseAddr);
    opts.maxRequestSize(PISTACHE_SERVER_MAX_REQUEST_SIZE);
    opts.maxResponseSize(PISTACHE_SERVER_MAX_RESPONSE_SIZE);
    endpoint.init(opts);


    router.addCustomHandler(Routes::bind(&ApiHandler::default_handler, this));
    endpoint.setHandler(router.handler());
    endpoint.serveThreaded();
};

/*****************************************************************************/
void ApiHandler::default_handler(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.send(Pistache::Http::Code::Not_Found,
        R"({"code": 404, "message": "The resource or method does not exist!"})");
}

/*****************************************************************************/
QJsonObject DigitalRooster::REST::qjson_form_std_string(
    const std::string& data) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonParseError perr;
    auto jd = QJsonDocument::fromJson(data.c_str(), &perr);
    if (perr.error != QJsonParseError::NoError) {
        throw std::invalid_argument(data + " is not valid JSON");
    }
    return jd.object();
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
