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

#include "AlarmApi.hpp"
#include "alarm.hpp"
#include "common.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "AlarmApi");

/*****************************************************************************/
AlarmApi::AlarmApi(IAlarmStore& as, Pistache::Rest::Router& router,
    QObject& ctx)
    : alarmstore(as)
    , context(ctx) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // Access list or create new station
    Routes::Get(router, API_URL_BASE + api_ressource,
        Routes::bind(&AlarmApi::read_alarm_list, this));
    Routes::Post(router, API_URL_BASE + api_ressource,
        Routes::bind(&AlarmApi::add_alarm, this));

    // Manage individual station identified by UUID
    Routes::Get(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&AlarmApi::get_alarm, this));
    Routes::Delete(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&AlarmApi::delete_alarm, this));
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void AlarmApi::read_alarm_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto all = run_in_thread_of(
            context, [this]() { return to_json_array(alarmstore.get_alarms()); });
        respond_json_array(all, request, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void AlarmApi::get_alarm(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto uid = uid_from_request(request);
        auto obj = run_in_thread_of(context,
            [this, uid]() { return alarmstore.get_alarm(uid)->to_json_object(); });
        respond_json_object(obj, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void AlarmApi::add_alarm(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto json = qjson_form_std_string(request.body());
        // create QObjects in the application thread
        auto id = run_in_thread_of(context, [this, json]() {
            auto item = Alarm::from_json_object(json);
            alarmstore.add_alarm(item);
            return item->get_id();
        });
        respond_SuccessCreated(id, response);
    });
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE] - Pistache API does a std::move down the line
void AlarmApi::delete_alarm(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    handle_request(response, [&]() {
        auto uid = uid_from_request(request);
        run_in_thread_of(context, [this, uid]() { alarmstore.delete_alarm(uid); });
        response.send(Pistache::Http::Code::Ok); // DELETE ok without MIME TYPE
    });
}
