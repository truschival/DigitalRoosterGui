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

#ifndef _MEDIAPLAYERPROXY_HPP_
#define _MEDIAPLAYERPROXY_HPP_

#include <QMediaPlayer>
#include <QObject>
#include <memory>

#include "mediaplayer.hpp"

namespace DigitalRooster {
class PlayableItem;
class PodcastEpisode;

/**
 * Wrapper around QMediaPlayer to allow access to the same player instance from
 * QML and C++
 */
class MediaPlayerProxy : public MediaPlayer {
    Q_OBJECT
public:
    MediaPlayerProxy();
    ~MediaPlayerProxy() = default;
    MediaPlayerProxy(const MediaPlayerProxy& rhs) = delete;
    MediaPlayerProxy& operator=(const MediaPlayerProxy& rhs) = delete;

    /**
     * Enable/Disable updates of position
     * QMediaPlayer stop for instance resets the media position to 0
     * before anything else in which case we lose the previous position.
     * @param enable
     */
    void enable_position_update(bool enable);
    bool is_position_updateable() const {
        return position_updateable;
    }

private:
    virtual bool is_seekable() const override;
    virtual bool is_muted() const;
    virtual int do_get_volume() const override;
    virtual qint64 do_get_duration() const override;
    virtual qint64 do_get_position() const override;
    virtual QMediaPlayer::MediaStatus do_media_status() const override;
    virtual QMediaPlayer::State do_playback_state() const override;
    virtual QMediaPlayer::Error do_error() const;
    virtual void do_set_media(
        std::shared_ptr<DigitalRooster::PlayableItem> media) override;
    virtual void do_set_playlist(QMediaPlaylist* playlist) override;
    virtual void do_set_position(qint64 position) override;
    virtual void do_set_muted(bool muted) override;
    virtual void do_set_volume(int volume) override;
    virtual void do_increment_volume(int increment) override;
    virtual void do_seek(qint64 incr) override;
    virtual void do_pause() override;
    virtual void do_play() override;
    virtual void do_stop() override;

    /**
     * Position should be updated periodically
     * fixes the unwanted behavior that QMediaPlayer emits a postionChanged() to 0
     * before anything else when stop is called
     */
    bool position_updateable{false};
    /**
     * Linear volume 0..100%
     * Initialized because increment/decrement has to work with some value
     */
    int linear_volume = 0;

    /**
     * The actual player implementation
     */
    std::unique_ptr<QMediaPlayer> backend;
    /**
     * currently selected media (Podcastepisode, RadioStream...)
     */
    std::shared_ptr<PlayableItem> current_item;
}; // Player
} // namespace DigitalRooster
#endif // _MEDIAPLAYERPROXY_HPP_
