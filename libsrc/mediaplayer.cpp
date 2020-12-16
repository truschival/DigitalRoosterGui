// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>

#include "mediaplayer.hpp"
#include "util.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.MediaPlayer");

/*****************************************************************************/
bool MediaPlayer::muted() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return is_muted();
}

/*****************************************************************************/
int MediaPlayer::get_volume() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_get_volume();
}

/*****************************************************************************/
void MediaPlayer::set_muted(bool muted) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_set_muted(muted);
}

/*****************************************************************************/
void MediaPlayer::set_volume(int volume) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << volume;
    if (!value_in_0_100(volume)) {
        qCWarning(CLASS_LC) << "invalid volume (must be 0..100%)";
        return;
    }
    return do_set_volume(volume);
}

/*****************************************************************************/
void MediaPlayer::increment_volume(int increment) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_increment_volume(increment);
}

/*****************************************************************************/
bool MediaPlayer::seekable() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return is_seekable();
}

/*****************************************************************************/
void MediaPlayer::seek(qint64 incr) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_seek(incr);
}

/*****************************************************************************/
qint64 MediaPlayer::get_position() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_get_position();
}

/*****************************************************************************/
void MediaPlayer::set_position(qint64 position) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_set_position(position);
}

/*****************************************************************************/
qint64 MediaPlayer::get_duration() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_get_duration();
}

/*****************************************************************************/
QMediaPlayer::MediaStatus MediaPlayer::media_status() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_media_status();
}

/*****************************************************************************/
QMediaPlayer::State MediaPlayer::playback_state() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_playback_state();
}

/*****************************************************************************/
QMediaPlayer::Error MediaPlayer::error() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_error();
}

/*****************************************************************************/
void MediaPlayer::set_media(
    std::shared_ptr<DigitalRooster::PlayableItem> media) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_set_media(std::move(media));
}

/*****************************************************************************/
void MediaPlayer::set_playlist(QMediaPlaylist* playlist) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_set_playlist(playlist);
}

/*****************************************************************************/
void MediaPlayer::pause() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_pause();
}

/*****************************************************************************/
void MediaPlayer::play() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_play();
}

/*****************************************************************************/
void MediaPlayer::stop() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return do_stop();
}
/*****************************************************************************/
