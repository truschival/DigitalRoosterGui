/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <QDateTime>
#include <QMediaPlayer>
#include <QObject>
#include <memory>


namespace DigitalRooster {
class PlayableItem;
class PodcastEpisode;
class RadioStream;

/**
 * Wrapper around QMediaPlayer to allow access to the same player instance from
 * QML and C++
 */
class Player : public QObject {
    Q_OBJECT
    Q_PROPERTY(qint64 position READ get_position)
    Q_PROPERTY(qint64 duration READ get_duration)
    Q_PROPERTY(int volume READ get_volume WRITE set_volume NOTIFY volume_changed)
    Q_PROPERTY(bool muted READ muted WRITE set_muted NOTIFY muted_changed)
    Q_PROPERTY(bool seekable READ seekable)
public:
	Player();
    ~Player() = default;
    Player(const Player& rhs) = delete;
    Player operator=(const Player& rhs) = delete;

    /**
     * Seek to position. Wrapper to set_position to implement same interface
	 * as QMLMediaPlayer. Has no effect if media is not seekable
	 * @param pos new postion
	 */
	Q_INVOKABLE
	void seek(qint64 pos) {
		if (this->seekable())
			this->set_position(pos);
	}
	;

	/** property access methods */
	bool seekable() const;
	bool muted() const;
	int get_volume() const;
	qint64 get_duration() const;
	qint64 get_position() const;

	/**
	 * Initial player volume
	 */
	const int initial_volume=5;
public slots:
    void set_media(RadioStream* media);
    void set_media(PodcastEpisode* media);

    void set_position(qint64 position);
    void set_muted(bool muted);
    void set_volume(int volume);
    void pause();
    void play();
    void stop();
signals:
    void position_changed(qint64 position);
    void media_changed(const QMediaContent& media);
    void volume_changed(int volume);
    void muted_changed(bool muted);
    void playback_state_changed(QMediaPlayer::State state);
	void error(QMediaPlayer::Error error);

private:
    /**
     * The actual player implementation
     */
    std::unique_ptr<QMediaPlayer> backend;

}; // Player
} // namespace DigitalRooster
#endif // _PLAYER_HPP_
