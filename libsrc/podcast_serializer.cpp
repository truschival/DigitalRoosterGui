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
#include "timeprovider.hpp"
using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSerializer");

/*****************************************************************************/
void PodcastSerializer::store_to_file(
    PodcastSource* ps, const QString& file_path) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    QJsonObject ps_obj = json_from_podcast_source(ps);
    QJsonArray episodes;
    for (const auto& episode : ps->get_episodes()) {
        episodes.append(json_from_episode(episode.get()));
    }
    ps_obj[KEY_EPISODES] = episodes;

    QSaveFile cache_file(file_path);
    try {
        cache_file.open(
            QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QJsonDocument doc(ps_obj);
        cache_file.write(doc.toJson());
        cache_file.commit();
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
}

/*****************************************************************************/
void PodcastSerializer::store_to_file(PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    store_to_file(ps, ps->get_cache_file_name());
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
    auto json_doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject tl_obj = json_doc.object();
    if (!tl_obj.isEmpty()) {
        parse_podcast_source_from_json(tl_obj, ps);
    } else {
        qCWarning(CLASS_LC) << "empty json document read from" << file_path;
        throw PodcastSourceJSonCorrupted("Document empty!");
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
    auto ts_str = tl_obj[KEY_TIMESTAMP].toString();
    QDateTime timestamp;
    if (ts_str != "") {
        timestamp = QDateTime::fromString(ts_str);
    }
    if (!timestamp.isValid()) {
        qCCritical(CLASS_LC)
            << "no valid timestamp in file - will not restore data";
        throw PodcastSourceJSonCorrupted("invalid timestamp");
    }
    if (ps->get_last_updated().isValid() &&
        ps->get_last_updated() > timestamp) {
        qCDebug(CLASS_LC) << "podcast source is newer than stored information";
    } else {
        // need to update podcast source data
        auto title = tl_obj[KEY_TITLE].toString();
        auto desc = tl_obj[KEY_DESCRIPTION].toString();
        auto img_url = tl_obj[KEY_ICON_URL].toString();
        auto img_cached = tl_obj[KEY_IMAGE_CACHE].toString();
        ps->set_title(title);
        ps->set_description(desc);
        ps->set_image_uri(img_url);
        ps->set_image_file_path(img_cached);
    }
    auto episodes_json_array = tl_obj[KEY_EPISODES].toArray();
    if (episodes_json_array.isEmpty()) {
        qCWarning(CLASS_LC)
            << "JSON for PodcastSource does not contain episodes";
        return;
    }
    /*
     * in either case read individual podcast episiode settings to at least set
     * episode positions
     */
    for (const auto& ep : episodes_json_array) {
        auto ep_ptr = parse_episode_from_json(ep.toObject());
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
QJsonObject PodcastSerializer::json_from_episode(
    const PodcastEpisode* episode) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject ep_obj;

    ep_obj[KEY_TITLE] = episode->get_title();
    ep_obj[KEY_URI] = episode->get_url().toString();
    ep_obj[KEY_DURATION] = episode->get_duration();
    ep_obj[KEY_POSITION] = episode->get_position();
    ep_obj[KEY_ID] = episode->get_guid();

    ep_obj[KEY_PUBLISHED] = episode->get_publication_date().toString();
    ep_obj[KEY_DESCRIPTION] = episode->get_description();
    ep_obj[KEY_PUBLISHER] = episode->get_publisher();
    return ep_obj;
}

/*****************************************************************************/
QJsonObject DigitalRooster::PodcastSerializer::json_from_podcast_source(
    const PodcastSource* ps) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QJsonObject ps_obj;
    ps_obj[KEY_TIMESTAMP] = wallclock->now().toString();
    ps_obj[KEY_TITLE] = ps->get_title();
    ps_obj[KEY_DESCRIPTION] = ps->get_description();
    ps_obj[KEY_ICON_URL] = ps->get_image_uri().toString();
    ps_obj[KEY_IMAGE_CACHE] = ps->get_image_file_path();
    return ps_obj;
}

/*****************************************************************************/
std::shared_ptr<PodcastEpisode> PodcastSerializer::parse_episode_json_impl(
    const QJsonObject& ep_obj) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto title = ep_obj[KEY_TITLE].toString();
    auto media_url = QUrl(ep_obj[KEY_URI].toString());
    auto ep = std::make_shared<PodcastEpisode>(title, media_url);
    ep->set_title(title);
    auto duration = ep_obj[KEY_DURATION].toInt(1);
    ep->set_duration(duration);
    auto position = ep_obj[KEY_POSITION].toInt(0);
    ep->set_position(position);

    ep->set_publication_date(
        QDateTime::fromString(ep_obj[KEY_PUBLISHED].toString()));
    ep->set_publisher(ep_obj[KEY_PUBLISHER].toString());
    ep->set_description(ep_obj[KEY_DESCRIPTION].toString());
    /* pubisher assinged id, can be url format hence a string not a QUuid */
    ep->set_guid(ep_obj[KEY_ID].toString());
    return ep;
}

/*****************************************************************************/
