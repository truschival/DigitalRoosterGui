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
RadioApi::RadioApi(IStationStore& station, Pistache::Rest::Router& router)
    : stationstore(station) {
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
void RadioApi::read_radio_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(stationstore.get_stations(), request, response);
}

/*****************************************************************************/
void RadioApi::get_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    try {
        // Massively ugly casting to get something convertible to QUuid...
        auto uid = QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
        QJsonDocument jd;
        auto result = stationstore.get_station(uid);
        jd.setObject(result->to_json_object());
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
        response.send(Pistache::Http::Code::Ok, jd.toJson().toStdString());
    } catch (std::out_of_range& oor) {
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
        // wrong UUID provided
        response.send(
            Pistache::Http::Code::Bad_Request, BAD_REQUEST_NO_ITEM_WITH_UUID);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        InternalErrorJson je(exc, 500);
        response.send(Pistache::Http::Code::Internal_Server_Error, je);
    }
}

/*****************************************************************************/
void RadioApi::add_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
        auto st = PlayableItem::from_json_object(
            qjson_form_std_string(request.body()));
        stationstore.add_radio_station(st);
        respond_SuccessCreated(st, response);
    } catch (std::invalid_argument& ia) {
        InternalErrorJson je(ia, 400);
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
        response.send(Pistache::Http::Code::Bad_Request, je);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        InternalErrorJson je(exc, 500);
        response.send(Pistache::Http::Code::Internal_Server_Error, je);
    }
}

/*****************************************************************************/
void RadioApi::delete_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        // Massively ugly casting to get something convertible to QUuid...
        auto uid = QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
        stationstore.delete_radio_station(uid);
        response.send(Pistache::Http::Code::Ok); // NO MIME TYPE!
    } catch (std::out_of_range& oor) {
        response.setMime(
             Pistache::Http::Mime::MediaType::fromString("application/json"));
        // wrong UUID provided
        response.send(
            Pistache::Http::Code::Bad_Request, BAD_REQUEST_NO_ITEM_WITH_UUID);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        InternalErrorJson je(exc, 500);
        response.send(Pistache::Http::Code::Internal_Server_Error, je);
    }
}
