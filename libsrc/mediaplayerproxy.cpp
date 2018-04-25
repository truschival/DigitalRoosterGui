/******************************************************************************
* \filename
* \brief
*
* \details
*
* \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
* \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
* 			 SPDX-License-Identifier: GPL-2.0-or-later}
*
*****************************************************************************/

#include <mediaplayerproxy.hpp>
#include <mediaplayerproxy.hpp>
#include <QDebug>
#include <QMediaPlayer>
#include "PlayableItem.hpp"

using namespace DigitalRooster;

/***********************************************************************/

MediaPlayerProxy::MediaPlayerProxy()
    : backend(std::make_unique<QMediaPlayer>()) {
    //qDebug() << Q_FUNC_INFO;
	backend->setVolume(initial_volume);
    QObject::connect(backend.get(), &QMediaPlayer::mediaChanged,
        [=](const QMediaContent& media) { emit media_changed(media); });

    QObject::connect(backend.get(), &QMediaPlayer::positionChanged,
        [=](qint64 position) { emit position_changed(position); });

    QObject::connect(backend.get(), &QMediaPlayer::volumeChanged,
        [=](int volume) { emit volume_changed(volume); });

    QObject::connect(backend.get(), &QMediaPlayer::mutedChanged,
        [=](bool muted) { emit muted_changed(muted); });

    QObject::connect(backend.get(), &QMediaPlayer::stateChanged,
        [=](QMediaPlayer::State state) { emit playback_state_changed(state); });

    QObject::connect(backend.get(), &QMediaPlayer::durationChanged,
        [=](qint64 duration) { emit duration_changed(duration); });

    QObject::connect(backend.get(), &QMediaPlayer::seekableChanged,
        [=](bool seekable) { emit seekable_changed(seekable); });

    QObject::connect(backend.get(), &QMediaPlayer::mediaStatusChanged,
        [=](QMediaPlayer::MediaStatus status) {
            emit media_status_changed(status);
        });
}

/*****************************************************************************/
bool MediaPlayerProxy::seekable() const {
	//qDebug() << Q_FUNC_INFO;
	return backend->isSeekable();
}

/*****************************************************************************/
qint64 MediaPlayerProxy::get_position() const {
	//qDebug() << Q_FUNC_INFO;
	return backend->position();
}

/*****************************************************************************/

QMediaPlayer::MediaStatus MediaPlayerProxy::media_status() const {
	//qDebug() << Q_FUNC_INFO;
	return backend->mediaStatus();
}

/*****************************************************************************/
qint64 MediaPlayerProxy::get_duration() const {
    //qDebug() << Q_FUNC_INFO;
    return backend->duration();
}

/*****************************************************************************/
void MediaPlayerProxy::set_position(qint64 position) {
    qDebug() << Q_FUNC_INFO << position;
    backend->setPosition(position);
}
/*****************************************************************************/
bool MediaPlayerProxy::muted() const {
    // qDebug() << Q_FUNC_INFO;
    return backend->isMuted();
}

/*****************************************************************************/
int MediaPlayerProxy::get_volume() const {
    // qDebug() << Q_FUNC_INFO;
    return backend->volume();
}
/*****************************************************************************/
void MediaPlayerProxy::set_media(DigitalRooster::RadioStream* media) {
    qDebug() << Q_FUNC_INFO;
    backend->setMedia(QMediaContent(media->get_url()));
}

/*****************************************************************************/
void MediaPlayerProxy::set_media(DigitalRooster::PodcastEpisode* media) {
    qDebug() << Q_FUNC_INFO;
    backend->setMedia(QMediaContent(media->get_url()));
}

/*****************************************************************************/
void MediaPlayerProxy::set_muted(bool muted) {
    // qDebug() << Q_FUNC_INFO;
    backend->setMuted(muted);
}

/*****************************************************************************/
QMediaPlayer::State MediaPlayerProxy::playback_state() const {
    return backend->state();
}

/*****************************************************************************/
void MediaPlayerProxy::set_volume(int volume) {
	//qDebug() << Q_FUNC_INFO;
	backend->setVolume(volume);
}
/*****************************************************************************/
void MediaPlayerProxy::pause() {
	qDebug() << Q_FUNC_INFO;
	backend->pause();
}
/*****************************************************************************/
void MediaPlayerProxy::play() {
	qDebug() << Q_FUNC_INFO;
	backend->play();
}
/*****************************************************************************/
void MediaPlayerProxy::stop() {
	qDebug() << Q_FUNC_INFO;
	backend->stop();
}
/*****************************************************************************/
