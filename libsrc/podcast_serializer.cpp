/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QFile>
#include <QLoggingCategory>
#include <QUuid>
#include <exception>

#include "PodcastSource.hpp"
#include "appconstants.hpp"
#include "podcast_serializer.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSerializer");

/*****************************************************************************/
void PodcastSerializer::store_to_file(
    PodcastSource* ps, const QString& file_path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void PodcastSerializer::store_to_file(PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void PodcastSerializer::read_from_file(
    PodcastSource* ps, const QString& file_path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCCritical(CLASS_LC) << file.errorString();
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory),
            file.errorString().toStdString());
    }
    auto json_doc = QJsonDocument::fromBinaryData(file.readAll());
    QJsonObject tl_obj = json_doc.object();
    if (!tl_obj.isEmpty()) {
        parse_podcast_source_from_json(tl_obj, ps);
    }
}

/*****************************************************************************/
void PodcastSerializer::read_from_file(PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    read_from_file(ps, ps->get_cache_file_name());
}

/*****************************************************************************/
void PodcastSerializer::parse_podcast_source_from_json(
    QJsonObject& tl_obj, PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // read podcast source configuration properties
    auto ts_str = tl_obj[KEY_TIMESSTAMP].toString();
    QDateTime timestamp;
    if (ts_str != "") {
        timestamp = QDateTime::fromString(ts_str);
    }
    if (!timestamp.isValid()) {
        qCCritical(CLASS_LC)
            << "no valid timestamp in file - will not restore data";
        return;
    }
    if (ps->get_last_updated().isValid() &&
        ps->get_last_updated() < timestamp) {
        qCDebug(CLASS_LC) << "podcast source is newer than stored information";
    } else {
        // need to update podcast source data
        auto name = tl_obj[KEY_NAME].toString();
        auto desc = tl_obj[KEY_DESCRIPTION].toString();
        auto img_src = tl_obj[KEY_IMAGE_PATH].toString();
        ps->set_title(name);
        ps->set_description(desc);
        ps->set_image_uri(img_src);
    }
    auto episodes_json_array = tl_obj[KEY_EPISODES].toArray();
    if (episodes_json_array.isEmpty()) {
        return;
    }
    /*
     * in either case read individual podcast episiode settings to at least set
     * episode positions
     */
    for (const auto& ep : episodes_json_array) {
        auto ep_ptr = parse_episode_json_impl(ep.toObject());
        auto existing_ep = ps->get_episode_by_id(ep_ptr->get_guid());
        if (existing_ep) { // found -> update position
            existing_ep->set_position(ep_ptr->get_position());
        } else { // not found -> add
            ps->add_episode(ep_ptr);
        }
    }
}

/*****************************************************************************/
std::shared_ptr<PodcastEpisode> PodcastSerializer::parse_episode_from_json(
    const QJsonObject& ep_obj) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return parse_episode_json_impl(ep_obj);
}

/*****************************************************************************/
std::shared_ptr<PodcastEpisode> PodcastSerializer::parse_episode_json_impl(
    const QJsonObject& ep_obj) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto display_name = ep_obj[KEY_NAME].toString();
    auto media_url = QUrl(ep_obj[KEY_URI].toString());
    auto ep = std::make_shared<PodcastEpisode>(display_name, media_url);

    auto position = ep_obj[KEY_POSITION].toInt(0);
    ep->set_position(position);

    auto duration = ep_obj[KEY_DURATION].toInt(0);
    ep->set_duration(duration);

    /* pubisher assinged id, can be url format hence a string not a QUuid */
    auto publisher_guid = ep_obj[KEY_ID].toString();
    if (!publisher_guid.isEmpty()) {
        ep->set_guid(publisher_guid);
    }
    return ep;
}

/*****************************************************************************/
