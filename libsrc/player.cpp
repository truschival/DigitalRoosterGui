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
#include <QMediaPlayer>
#include "PlayableItem.hpp"
#include "player.hpp"

using namespace DigitalRooster;

/*****************************************************************************/

Player::Player() : backend(std::make_unique<QMediaPlayer>()) {
	qDebug() << __FUNCTION__;
	QObject::connect(backend.get(), &QMediaPlayer::mediaChanged, [=](const QMediaContent &media) {
		emit media_changed(media);
	});

	QObject::connect(backend.get(), &QMediaPlayer::positionChanged, [=](qint64 position) {
		emit position_changed(position);
	});

	QObject::connect(backend.get(), &QMediaPlayer::volumeChanged, [=](int volume) {
		emit volume_changed(volume);
	});

	QObject::connect(backend.get(), &QMediaPlayer::mutedChanged, [=](bool muted) {
		emit muted_changed(muted);
	});

	QObject::connect(backend.get(), &QMediaPlayer::stateChanged, [=](QMediaPlayer::State state) {
		emit playback_state_changed(state);
	});
}


/*****************************************************************************/
bool Player::seekable() const {
	qDebug() << __FUNCTION__;
	return backend->isSeekable();
}

/*****************************************************************************/

qint64 Player::get_position() const {
	qDebug() << __FUNCTION__;
	return backend->position();
}
/*****************************************************************************/

qint64 Player::get_duration() const {
	qDebug() << __FUNCTION__;
	return backend->duration();
}

/*****************************************************************************/

void Player::set_position(qint64 position) {
	qDebug() << __FUNCTION__;
	backend->setPosition(position);
}

/*****************************************************************************/

int Player::seek(qint64 pos) {
	qDebug() << __FUNCTION__;

	return -1;
}
/*****************************************************************************/

void  Player::set_media(RadioStream* media) {
	qDebug() << __FUNCTION__;
}

/*****************************************************************************/
void Player::set_media(PodcastEpisode* media) {
	qDebug() << __FUNCTION__;
	backend->setMedia(QMediaContent(media->get_url()));
}

/*****************************************************************************/
void Player::set_muted(bool muted) {
	qDebug() << __FUNCTION__;
}
/*****************************************************************************/
void Player::set_volume(int volume) {
	qDebug() << __FUNCTION__;
}
/*****************************************************************************/
void Player::pause() {
	qDebug() << __FUNCTION__;
	backend->pause();
}
/*****************************************************************************/
void Player::play() {
	qDebug() << __FUNCTION__;
	backend->play();
}
/*****************************************************************************/
void Player::stop() {
	qDebug() << __FUNCTION__;
	backend->stop();
}
/*****************************************************************************/