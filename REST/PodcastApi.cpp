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

#include "PodcastApi.hpp"
#include "PodcastSource.hpp"

#include "common.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "PodcastApi");

/*****************************************************************************/
PodcastApi::PodcastApi(IPodcastStore& ps, Pistache::Rest::Router& router)
    : podcaststore(ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // Access list or create new podcast
    Routes::Get(router, API_URL_BASE + api_ressource,
        Routes::bind(&PodcastApi::read_podcast_list, this));
    Routes::Post(router, API_URL_BASE + api_ressource,
        Routes::bind(&PodcastApi::add_podcast, this));

    // Manage individual podcast identified by UUID
    Routes::Get(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&PodcastApi::get_podcast, this));
    Routes::Delete(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&PodcastApi::delete_podcast, this));
}

/*****************************************************************************/
void PodcastApi::read_podcast_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(podcaststore.get_podcast_sources(), request, response);
}

/*****************************************************************************/
void PodcastApi::get_podcast(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    try {
        // Massively ugly casting to get something convertible to QUuid...
        auto uid = QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
        QJsonDocument jd;
        auto result = podcaststore.get_podcast_source(uid);
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
        response.send(Pistache::Http::Code::Internal_Server_Error, exc.what());
    }
}

/*****************************************************************************/
void PodcastApi::add_podcast(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        auto ps = PodcastSource::from_json_object(
            qjson_form_std_string(request.body()));
        podcaststore.add_podcast_source(ps);
        respond_SuccessCreated(ps, response);
    } catch (std::invalid_argument& ia) {
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
        response.send(Pistache::Http::Code::Bad_Request, ia.what());
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        response.send(Pistache::Http::Code::Internal_Server_Error, exc.what());
    }
}

/*****************************************************************************/
void PodcastApi::delete_podcast(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        // Massively ugly casting to get something convertible to QUuid...
        auto uid = QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
        podcaststore.delete_podcast_source(uid);
        response.send(Pistache::Http::Code::Ok); // NO Mimetype when delete OK
    } catch (std::out_of_range& oor) {
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
    	// wrong UUID provided
        response.send(
            Pistache::Http::Code::Bad_Request, BAD_REQUEST_NO_ITEM_WITH_UUID);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        response.send(Pistache::Http::Code::Internal_Server_Error, exc.what());
    }
}
