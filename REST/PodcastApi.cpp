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
PodcastApi::PodcastApi(IPodcastStore& ps, Pistache::Rest::Router& router,
    QObject& ctx)
    : podcaststore(ps)
    , context(ctx) {
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
// coverity[PASS_BY_VALUE]
void PodcastApi::read_podcast_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto all = run_in_thread_of(
            context, [this]() { return to_json_array(podcaststore.get_podcast_sources()); });
        respond_json_array(all, request, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void PodcastApi::get_podcast(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto uid = uid_from_request(request);
        auto obj = run_in_thread_of(context,
            [this, uid]() { return podcaststore.get_podcast_source(uid)->to_json_object(); });
        respond_json_object(obj, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void PodcastApi::add_podcast(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto json = qjson_form_std_string(request.body());
        // create QObjects in the application thread
        auto id = run_in_thread_of(context, [this, json]() {
            auto item = PodcastSource::from_json_object(json);
            podcaststore.add_podcast_source(item);
            return item->get_id();
        });
        respond_SuccessCreated(id, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE] - Pistache API does a std::move down the line
void PodcastApi::delete_podcast(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto uid = uid_from_request(request);
        run_in_thread_of(context, [this, uid]() { podcaststore.delete_podcast_source(uid); });
        response.send(Pistache::Http::Code::Ok); // DELETE ok without MIME TYPE
    });
}
