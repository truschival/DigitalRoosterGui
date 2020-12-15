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

#include <memory>
#include <sstream>
#include <string>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
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
        const Pistache::Optional<std::string>& query, int min, int max);

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
     * @tparam T some container type
     * @tparam T
     * @param all container with pointers to objects that implement
     * to_json_object()
     * @param request query with possibly "length" and "offset" parameters
     * @param response output writer
     */
    template <typename T>
    void respond_json_array(const T& all,
        const Pistache::Rest::Request& request,
        Pistache::Http::ResponseWriter& response) {

        auto max_size = all.size();
        int offset = 0;
        auto offset_param = request.query().get("offset");
        if (!offset_param.isEmpty()) {
            // Offset between 0 and max_size
            offset = get_val_from_query_within_range(offset_param, 0, max_size);
        }

        int length = max_size;
        auto length_param = request.query().get("length");
        if (!length_param.isEmpty()) {
            // length between 0 and max_size-offset
            length = get_val_from_query_within_range(
                length_param, 1, max_size - offset);
        }

        try {
            QJsonArray j;
            for (const auto& p : container_from_range(all, offset, length)) {
                j.push_back(QJsonValue(p->to_json_object()));
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
     * @tparam T a (smart) pointer to an object that implemntes QUuid get_id()
     * @param item created item
     * @param response
     */
    template <typename T>
    void respond_SuccessCreated(
        const T& item, Pistache::Http::ResponseWriter& response) {
        QJsonDocument jd;
        QJsonObject o;
        o["id"] = item->get_id().toString(QUuid::WithoutBraces);
        jd.setObject(o);
        response.setMime(Pistache::Http::Mime::MediaType::fromString("application/json"));
        response.send(Pistache::Http::Code::Ok, jd.toJson().toStdString());
    }


} // namespace REST
} // namespace DigitalRooster
#endif /* _REST_COMMON_HPP_ */
