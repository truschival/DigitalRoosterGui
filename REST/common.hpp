/******************************************************************************
 * \filename
 * \brief     REST adapter for configuration
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _REST_COMMON_HPP_
#define _REST_COMMON_HPP_

#include <chrono>
#include <future>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QMetaObject>
#include <QObject>
#include <QThread>
#include <QUuid>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

namespace DigitalRooster {
namespace REST {

    /**
     * API base path prepended to all REST resources
     * e.g. http://digitalrooster/<base>/radio
     */
    const std::string API_URL_BASE = "/api/1.0/";

    /**
     * Reponse to send if we didn't find a element with given UUID
     */
    const std::string BAD_REQUEST_NO_ITEM_WITH_UUID =
        R"({"code":400, "message": "no item for this UUID"})";

    /**
     * Maximum time a REST request waits for the application thread
     */
    const std::chrono::seconds APP_THREAD_TIMEOUT(5);

    /**
     * Execute a function in the thread of a QObject, i.e. the Qt main thread,
     * and wait for its result.
     * Pistache handlers run in worker threads but the configuration and its
     * QObjects must only be accessed from the main thread.
     * Exceptions thrown by f are rethrown in the calling thread.
     * @param context object living in the target thread
     * @param f callable, has to capture request data by value because it may
     *        still be executed after a timeout
     * @return result of f
     * @throws std::runtime_error if the main thread does not respond in time
     */
    template <typename F>
    auto run_in_thread_of(QObject& context, F f) -> std::invoke_result_t<F> {
        using R = std::invoke_result_t<F>;
        if (QThread::currentThread() == context.thread()) {
            return f();
        }
        auto task = std::make_shared<std::packaged_task<R()>>(std::move(f));
        auto result = task->get_future();
        QMetaObject::invokeMethod(
            &context, [task]() { (*task)(); }, Qt::QueuedConnection);
        if (result.wait_for(APP_THREAD_TIMEOUT) != std::future_status::ready) {
            throw std::runtime_error("application did not respond in time");
        }
        return result.get();
    }

    /**
     * Serialize all items of a container in a JSON array
     * @tparam T container with pointers to objects that implement
     * to_json_object()
     * @param all container
     * @return JSON array of all items
     */
    template <typename T>
    QJsonArray to_json_array(const T& all) {
        QJsonArray j;
        for (const auto& p : all) {
            j.push_back(QJsonValue(p->to_json_object()));
        }
        return j;
    }

    /**
     * Helper structure to package exceptions and reformat them into
     * JSON messages
     */
    struct InternalErrorJson {
        explicit InternalErrorJson(std::exception& exc, int code = 400)
            : message(exc.what())
            , error_code(code) {
        }
        std::string message;
        int error_code;

        operator std::string() const {
            std::stringstream ss;
            ss << "{";
            ss << R"("code":)" << error_code;
            ss << R"(,"message":")" << message;
            ss << R"("})";
            return ss.str();
        }
    };

    /**
     * Helper function to read a range with offset and length from T
     * and return this range as a copy
     * @param all a container with linear access iterators
     * @param offset start index
     * @param length max number of items to read (if available)
     * @return A container T with items form @ref all
     */
    template <typename T>
    T container_from_range(
        const T& all, unsigned long offset, unsigned long length) {
        auto start_it = all.begin() + offset;
        auto end_it = std::min(start_it + length, all.end());
        return T(start_it, end_it);
    }
    /*****************************************************************************/

    /**
     * Helper function to get a  variable from optional query  parameters such
     * as length or offset
     * @param query Pistache HTTP query that may or may not contain a numeric
     * value for variable
     * @param min - minimum value (returned if value form query less than min)
     * @param max - maximum value (returned if value from query greater than
     * max)
     * @return value within  range
     */
    int get_val_from_query_within_range(
        const std::optional<std::string>& query, int min, int max);

    /**
     * Convenience function to get a QJsonObject from a std::string
     * @throws std::invalid_argument if string is not json parsable
     * @param data some json string
     * @return a QJsonObject
     */
    QJsonObject qjson_form_std_string(const std::string& data);

    /**
     * Simple Helper function that creates a HTTP response with a JSON array of
     * the requested objects
     * @param all JSON array of all items
     * @param request query with possibly "length" and "offset" parameters
     * @param response output writer
     */
    inline void respond_json_array(const QJsonArray& all,
        const Pistache::Rest::Request& request,
        Pistache::Http::ResponseWriter& response) {

        int max_size = all.size();
        int offset = 0;
        auto offset_param = request.query().get("offset");
        if (offset_param.has_value()) {
            // Offset between 0 and max_size
            offset = get_val_from_query_within_range(offset_param, 0, max_size);
        }

        int length = max_size;
        auto length_param = request.query().get("length");
        if (length_param.has_value()) {
            // length between 0 and max_size-offset
            length = get_val_from_query_within_range(
                length_param, 1, max_size - offset);
        }

        try {
            QJsonArray j;
            for (int i = offset; i < std::min(offset + length, max_size); i++) {
                j.push_back(all.at(i));
            }
            QJsonDocument jdoc;
            jdoc.setArray(j);
            response.setMime(Pistache::Http::Mime::MediaType::fromString("application/json"));
            response.send(
                Pistache::Http::Code::Ok, jdoc.toJson().toStdString());
        } catch (std::exception& e) {
            InternalErrorJson je(e, 500);
            // send a 500 error - NO MIME TYPE!
            response.send(Pistache::Http::Code::Internal_Server_Error, je);
            return;
        }
    }
    /*****************************************************************************/

    /**
     * Helper function to make a correct SuccessCreated JSON response with the
     * unique id as only content. Used for creation of Alarm, podcastSources and
     * PlayableItem i.e. RadioStations
     * @param id unique id of created item
     * @param response
     */
    inline void respond_SuccessCreated(
        const QUuid& id, Pistache::Http::ResponseWriter& response) {
        QJsonDocument jd;
        QJsonObject o;
        o["id"] = id.toString(QUuid::WithoutBraces);
        jd.setObject(o);
        response.setMime(Pistache::Http::Mime::MediaType::fromString("application/json"));
        response.send(Pistache::Http::Code::Ok, jd.toJson().toStdString());
    }

    /**
     * Send a single JSON object
     * @param obj content
     * @param response
     */
    inline void respond_json_object(
        const QJsonObject& obj, Pistache::Http::ResponseWriter& response) {
        QJsonDocument jd(obj);
        response.setMime(
            Pistache::Http::Mime::MediaType::fromString("application/json"));
        response.send(Pistache::Http::Code::Ok, jd.toJson().toStdString());
    }

    /**
     * Read the UUID path parameter ":uid" of a request
     * @param request
     * @return uuid, null if not parsable
     */
    inline QUuid uid_from_request(const Pistache::Rest::Request& request) {
        return QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
    }

    /**
     * Run a request handler and translate exceptions into error responses
     * @param response output writer
     * @param f handler function
     */
    template <typename F>
    void handle_request(Pistache::Http::ResponseWriter& response, F f) {
        try {
            f();
        } catch (std::out_of_range&) {
            // wrong UUID provided
            response.setMime(Pistache::Http::Mime::MediaType::fromString(
                "application/json"));
            response.send(Pistache::Http::Code::Bad_Request,
                BAD_REQUEST_NO_ITEM_WITH_UUID);
        } catch (std::invalid_argument& ia) {
            InternalErrorJson je(ia, 400);
            response.setMime(Pistache::Http::Mime::MediaType::fromString(
                "application/json"));
            response.send(Pistache::Http::Code::Bad_Request, je);
        } catch (std::exception& exc) {
            // some other error occurred -> 500
            InternalErrorJson je(exc, 500);
            response.send(Pistache::Http::Code::Internal_Server_Error, je);
        }
    }


} // namespace REST
} // namespace DigitalRooster
#endif /* _REST_COMMON_HPP_ */
