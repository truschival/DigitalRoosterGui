/******************************************************************************
 * \filename
 * \brief   Interface for an abstract mediaplayer used to mock MediaPlayerProxy
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QDebug>
#include "mediaplayer.hpp"
using namespace DigitalRooster;

/*****************************************************************************/
bool MediaPlayer::muted() const {
    return is_muted();
}

/*****************************************************************************/
int MediaPlayer::get_volume() const {
    return do_get_volume();
}

/*****************************************************************************/
void MediaPlayer::set_muted(bool muted) {
    return do_set_muted(muted);
}

/*****************************************************************************/
void MediaPlayer::set_volume(int volume) {
    return do_set_volume(volume);
}

/*****************************************************************************/
bool MediaPlayer::seekable() const {
    return is_seekable();
}

/*****************************************************************************/
void MediaPlayer::seek(qint64 incr) {
    return do_seek(incr);
}

/*****************************************************************************/
qint64 MediaPlayer::get_position() const {
    return do_get_position();
}

/*****************************************************************************/
void MediaPlayer::set_position(qint64 position) {
    return do_set_position(position);
}

/*****************************************************************************/
qint64 MediaPlayer::get_duration() const {
    return do_get_duration();
}

/*****************************************************************************/
QMediaPlayer::MediaStatus MediaPlayer::media_status() const {
    return do_media_status();
}

/*****************************************************************************/
QMediaPlayer::State MediaPlayer::playback_state() const {
    return do_playback_state();
}

/*****************************************************************************/
QMediaPlayer::Error MediaPlayer::error() const {
    return do_error();
}

/*****************************************************************************/
void MediaPlayer::set_media(
    std::shared_ptr<DigitalRooster::PlayableItem> media) {
    return do_set_media(media);
}

/*****************************************************************************/
void MediaPlayer::set_playlist(QMediaPlaylist* playlist) {
    return do_set_playlist(playlist);
}

/*****************************************************************************/
void MediaPlayer::pause() {
	//qDebug() << Q_FUNC_INFO;
	return do_pause();
}

/*****************************************************************************/
void MediaPlayer::play() {
	//qDebug() << Q_FUNC_INFO;
	return do_play();
}

/*****************************************************************************/
void MediaPlayer::stop() {
	//qDebug() << Q_FUNC_INFO;
	return do_stop();
}
/*****************************************************************************/
