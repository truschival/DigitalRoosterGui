// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLoggingCategory>

#include <memory>
#include <string>
#include <optional>

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
// default dtor for PIMPL
DigitalRooster::RestApi::~RestApi() = default;

/*****************************************************************************/
ApiHandler::ApiHandler(DigitalRooster::IWeatherConfigStore& ws,
    DigitalRooster::IAlarmStore& as, DigitalRooster::IPodcastStore& ps,
    DigitalRooster::IStationStore& sts, DigitalRooster::ITimeOutStore& tos,
    Pistache::Address addr)
    : endpoint(addr)
    , alarmapi(as, router, app_thread_context)
    , radioapi(sts, router, app_thread_context)
    , podcastsapi(ps, router, app_thread_context) {
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
ApiHandler::~ApiHandler() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    endpoint.shutdown();
}

/*****************************************************************************/
void ApiHandler::default_handler(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.setMime(
        Pistache::Http::Mime::MediaType::fromString("application/json"));
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
        // do not echo the request body
        throw std::invalid_argument("request is not valid JSON: " +
            perr.errorString().toStdString() + " at offset " +
            std::to_string(perr.offset));
    }
    return jd.object();
}

/*****************************************************************************/
int DigitalRooster::REST::get_val_from_query_within_range(
    const std::optional<std::string>& query, int min, int max) {
    return get_val_from_query_within_range(query, min, max, min);
}

/*****************************************************************************/
int DigitalRooster::REST::get_val_from_query_within_range(
    const std::optional<std::string>& query, int min, int max, int fallback) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    int val = fallback;
    try {
        val = std::stoi(query.value());
    } catch (const std::bad_optional_access& e) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << e.what();
    } catch (const std::invalid_argument& e) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << e.what();
    } catch (const std::out_of_range& e) {
        // number does not fit into int, cap at the matching limit
        val = (query.value().find('-') != std::string::npos) ? min : max;
    }
    // cap val at lower end
    val = (val < min) ? min : val;
    // vap val at upper limit
    val = (val > max) ? max : val;
    return val;
}
