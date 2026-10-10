// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <pistache/endpoint.h>
#include <pistache/http.h>

#include "PlayableItem.hpp"
#include "RadioApi.hpp"
#include "common.hpp"
#include "util.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "RadioAPI");

/*****************************************************************************/
RadioApi::RadioApi(IStationStore& station, Pistache::Rest::Router& router,
    QObject& ctx)
    : stationstore(station)
    , context(ctx) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // Access list or create new station
    Routes::Get(router, API_URL_BASE + api_ressource,
        Routes::bind(&RadioApi::read_radio_list, this));
    Routes::Post(router, API_URL_BASE + api_ressource,
        Routes::bind(&RadioApi::add_station, this));

    // Manage individual station identified by UUID
    Routes::Get(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&RadioApi::get_station, this));
    Routes::Delete(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&RadioApi::delete_station, this));
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void RadioApi::read_radio_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto all = run_in_thread_of(
            context, [this]() { return to_json_array(stationstore.get_stations()); });
        respond_json_array(all, request, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void RadioApi::get_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto uid = uid_from_request(request);
        auto obj = run_in_thread_of(context,
            [this, uid]() { return stationstore.get_station(uid)->to_json_object(); });
        respond_json_object(obj, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void RadioApi::add_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto json = qjson_form_std_string(request.body());
        // create QObjects in the application thread
        auto id = run_in_thread_of(context, [this, json]() {
            auto item = PlayableItem::from_json_object(json);
            stationstore.add_radio_station(item);
            return item->get_id();
        });
        respond_SuccessCreated(id, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE] - Pistache API does a std::move down the line
void RadioApi::delete_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto uid = uid_from_request(request);
        run_in_thread_of(context, [this, uid]() { stationstore.delete_radio_station(uid); });
        response.send(Pistache::Http::Code::Ok); // DELETE ok without MIME TYPE
    });
}
