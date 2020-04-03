/******************************************************************************
 * \filename
 * \brief     API for managing Internet Radio Stations
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

#include <pistache/endpoint.h>
#include <pistache/http.h>

#include "PlayableItem.hpp"
#include "common.hpp"
#include "RadioAPI.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.RadioAPI");

/*****************************************************************************/
RadioAPI::RadioAPI(IStationStore& station, Pistache::Rest::Router& router)
    : stationstore(station) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // Access list or create new station
    Routes::Get(router, API_URL_BASE + api_ressource,
        Routes::bind(&RadioAPI::read_radio_list, this));
    Routes::Post(router, API_URL_BASE + api_ressource,
        Routes::bind(&RadioAPI::add_station, this));

    // Manage individual station identified by UUID
    Routes::Get(router, API_URL_BASE + api_ressource + ":uid",
            Routes::bind(&RadioAPI::get_station, this));
    Routes::Put(router, API_URL_BASE + api_ressource + ":uid",
        Routes::bind(&RadioAPI::update_station, this));
    Routes::Delete(router, API_URL_BASE + api_ressource + ":uid",
            Routes::bind(&RadioAPI::delete_station, this));

}

/*****************************************************************************/
void RadioAPI::read_radio_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(stationstore.get_stations(), request, response);
}


/*****************************************************************************/
void RadioAPI::get_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.send(Http::Code::Not_Implemented, "method not implemented!");
}

/*****************************************************************************/
void RadioAPI::add_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.send(Http::Code::Not_Implemented, "method not implemented!");
}

/*****************************************************************************/
void RadioAPI::update_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.send(Http::Code::Not_Implemented, "method not implemented!");
}

/*****************************************************************************/
void RadioAPI::delete_station(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    response.send(Http::Code::Not_Implemented, "method not implemented!");
}
