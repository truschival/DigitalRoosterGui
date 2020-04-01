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
     * Simple Helper function that creates a HTTP response with a JSON array of
     * the requested objects
     * @tparam T some container type
     * @tparam T
     * @param all container with pointers to objects that implement
     * to_json_object()
     * @param request query with possibly "length" and "offset" parameters
     * @param response output writer
     */
    /*****************************************************************************/
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
            response.send(
                Pistache::Http::Code::Ok, jdoc.toJson().toStdString());
        } catch (std::exception& e) {
            // send a 500 error
            response.send(
                Pistache::Http::Code::Internal_Server_Error, e.what());
            return;
        }
    }
} // namespace REST
} // namespace DigitalRooster
#endif /* _REST_COMMON_HPP_ */
