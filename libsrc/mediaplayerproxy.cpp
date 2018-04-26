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

#include <QDebug>
#include <QAudio>	
#include <QMediaPlayer>
#include "PlayableItem.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

/***********************************************************************/

MediaPlayerProxy::MediaPlayerProxy()
    : backend(std::make_unique<QMediaPlayer>()) {
    //qDebug() << Q_FUNC_INFO;
	backend->setVolume(initial_volume);
    QObject::connect(backend.get(), &QMediaPlayer::mediaChanged,
        [=](const QMediaContent& media) { emit media_changed(media); });

    QObject::connect(backend.get(), &QMediaPlayer::positionChanged,
        [=](qint64 position) { emit position_changed(position); 
		if (current_item.get() != nullptr) {
			current_item->set_position(position);
		}	
	});

	QObject::connect(backend.get(), static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
		[=](QMediaPlayer::Error  err) { 
		qDebug() << "Player Error" << err;
		emit error(err); 
	});
	
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
    // qDebug() << Q_FUNC_INFO << position;
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
void MediaPlayerProxy::set_media(std::shared_ptr<DigitalRooster::PlayableItem> media) {
    qDebug() << Q_FUNC_INFO;
	current_item = media;
	auto previous_position = media->get_position();
	qDebug() << media->get_url();
	backend->setMedia(QMediaContent(media->get_url()));
	if (previous_position != 0 && previous_position < media->get_duration()) {
		qDebug() << "restarting from position" << previous_position;
		set_position(previous_position);
	}
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
	// volumeSliderValue is in the range [0..100]

	auto linearVolume = QAudio::convertVolume(volume / qreal(100.0),
		QAudio::LogarithmicVolumeScale,
		QAudio::LinearVolumeScale);
	qDebug() << "Updating volume to " << qRound(linearVolume * 100);
	backend->setVolume(qRound(linearVolume * 100));
}
/*****************************************************************************/
void MediaPlayerProxy::pause() {
	//qDebug() << Q_FUNC_INFO;
	backend->pause();
}
/*****************************************************************************/
void MediaPlayerProxy::play() {
	//qDebug() << Q_FUNC_INFO;
	backend->play();
}
/*****************************************************************************/
void MediaPlayerProxy::stop() {
	//qDebug() << Q_FUNC_INFO;
	backend->stop();
}
/*****************************************************************************/
