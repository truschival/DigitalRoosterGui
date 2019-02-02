/******************************************************************************
 * \filename
 * \brief writes a podcast source to filesystem or restores its configuration
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _PODCASTSERIALIZER_HPP_
#define _PODCASTSERIALIZER_HPP_

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

namespace DigitalRooster {
class PodcastSource;

class PodcastSerializer : QObject {
    Q_OBJECT
public:
    /**
     * serializes podcast source to filesystem
     * @param ps podcastsource to write
     * @param file_path file to write
     */
    void store_to_file(PodcastSource* ps, const QString& file_path);

    /**
     * serializes podcast source to filesystem using
     * default file path
     * @param ps podcastsource to write
     */
    void store_to_file(PodcastSource* ps);

    /**
     * restore configuration of podcastsource form filesystem
     * @param ps podcastsource to restore
     * @param filename file to read
     */
    void read_from_file(PodcastSource* ps, const QString& file_path);

    /**
     * read configuration of podcast source to filesystem using
     * default file path
     * @param ps podcastsource to write
     */
    void read_from_file(PodcastSource* ps);

    /**
     * parse file and configure podcastsource accordingly
     * @param tl_obj top level object = PodcastSource representation
     * @param ps podcast source to configure
     */
    void parse_podcast_source_from_json(QJsonObject& tl_obj, PodcastSource* ps);

    /**
     * Read a single podcast episode object from file
     * NVI to allow for test-mocking
     * @param ep_obj JSON representation of a podcast Episode
     * @return PodcastEpisode object
     */
    std::shared_ptr<PodcastEpisode> parse_episode_from_json(
        const QJsonObject& ep_obj);

private:
    /**
     * Implementation of parse_episode_from_json
     * @param ep_obj JSON representation of a podcast Episode
     * @return PodcastEpisode object
     */
    std::shared_ptr<PodcastEpisode> parse_episode_json_impl(
        const QJsonObject& ep_obj);
};
} // namespace DigitalRooster
#endif // _PODCASTSERIALIZER_HPP_
