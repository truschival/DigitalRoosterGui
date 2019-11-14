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

#include <exception>

namespace DigitalRooster {
class PodcastSource;

/**
 * Serialization/Deserialization of PodcastSources and PodcastEpisodes
 * from filesystem
 */
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
     * @param file_path file to read
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

    /**
     * Create a JSON Object representation of a \ref PodcastEpisode
     * @param episode PodcastEpisode to serialize
     * @return JSON Object representation
     */
    QJsonObject json_from_episode(const PodcastEpisode* episode);

    /**
     * Create a JSON Object representation of a PodcastSource
     * @param ps the PodcastSource to serialize
     * @return JSON Object representation
     */
    QJsonObject json_from_podcast_source(const PodcastSource* ps);

private:
    /**
     * Implementation of parse_episode_from_json
     * @param ep_obj JSON representation of a podcast Episode
     * @return PodcastEpisode object
     */
    std::shared_ptr<PodcastEpisode> parse_episode_json_impl(
        const QJsonObject& ep_obj);
};

/**
 * Exception thrown if serialized podcast source is corrupted,
 */
class PodcastSourceJSonCorrupted : public std::runtime_error {
public:
    explicit PodcastSourceJSonCorrupted(const char* what)
        : std::runtime_error(what){};

    explicit PodcastSourceJSonCorrupted(const std::string& what)
        : std::runtime_error(what){};
};

} // namespace DigitalRooster
#endif // _PODCASTSERIALIZER_HPP_
