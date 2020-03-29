/******************************************************************************
 * \filename
 * \brief  Serialization/Deserialization of PodcastSources and PodcastEpisodes
 * from filesystem and/or JSON
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

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTimer>

#include <exception>
#include <memory>


namespace DigitalRooster {
class PodcastSource;

/**
 * Serialization/Deserialization of PodcastSources and PodcastEpisodes
 * from filesystem
 */
class PodcastSerializer : public QObject {
    Q_OBJECT
public:
    /**
     * Constructor
     * @param app_cache_dir
     * @param source optional PodcastSource can be set by
     *        \ref set_podcast_source later
     * @param delay duration after which write occurs
     */
    explicit PodcastSerializer(const QDir& app_cache_dir,
        PodcastSource* source = nullptr,
        std::chrono::milliseconds delay = std::chrono::milliseconds(1000));

    /**
     * Update PodcastSource
     */
    void set_podcast_source(PodcastSource* source);

    /**
     * Need destructor to stop timer
     */
    ~PodcastSerializer();
    PodcastSerializer(const PodcastSerializer&) = delete;
    PodcastSerializer(PodcastSerializer&&) = delete;
    PodcastSerializer& operator=(const PodcastSerializer&) = delete;
    PodcastSerializer& operator=(PodcastSerializer&&) = delete;

public slots:
    /**
     * Delete local cache file
     */
    void delete_cached_info();

    /**
     * Triggers a delayed write
     */
    void delayed_write();

    /**
     * Restore data in podcast source form file
     * if data in file is more recent/complete
     */
    void restore_info();
    /**
     * Immediately write data to cache file
     */
    void write();

private:
    /**
     * Podcast Source to serialize/de-serialize
     */
    PodcastSource* ps;

    /**
     * Timer for delayed write operations
     */
    QTimer writeTimer;

    /**
     * Cache directory
     */
    const QDir& cache_dir;

    /**
     * NVI implementation of delete_cached_info()
     */
    virtual void delete_cache();
    /**
     * NVI implementation of write()
     */
    virtual void write_cache();
};

/**
 * serializes podcast source to filesystem
 * @param ps podcastsource to write
 * @param file_path file to write
 */
void store_to_file(PodcastSource* ps, const QString& file_path);

/**
 * restore configuration of podcastsource form filesystem
 * @param ps podcastsource to restore
 * @param file_path file to read
 */
void read_from_file(PodcastSource* ps, const QString& file_path);

/**
 * parse file and configure podcastsource accordingly
 * @param tl_obj top level object = PodcastSource representation
 * @param ps podcast source to configure
 */
void parse_podcast_source_from_json(const QJsonObject& tl_obj, PodcastSource* ps);

/**
 * Reads episodes array from \ref json and updates \ref ps
 * if episode already exists we update the position information
 * @param json object with cached information of podcast source and episodes
 * @param ps PodcastSource to update
 */
void read_episodes_cache(const QJsonObject& json, PodcastSource* ps);

/**
 * Create a JSON Object representation of a PodcastSource
 * including all cached/dynamic information form RSS feed
 * @param ps the PodcastSource to serialize
 * @return JSON Object representation
 */
QJsonObject json_from_podcast_source(const PodcastSource* ps);

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
