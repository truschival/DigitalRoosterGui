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

#ifndef  _PLAYER_HPP_
#define  _PLAYER_HPP_

#include <QObject>
#include <QDateTime>
#include <QMediaPlayer>
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
		Q_PROPERTY(bool  seekable READ seekable)
    public:
		/**
		 * Enum to make player behave like QML MediaPlayer
		 */
		enum PlaybackState {
			PlayingState,
			PausedState,
			StoppedState
		};

		Player();
		~Player();
		Player(const Player& rhs) = delete;
		Player operator =(const Player& rhs) = delete;

		/**
		 * Seek to position
		 * @param pos new postion
		 * @return 0 for success -1 if failed (because not seekable)
		 */
		Q_INVOKABLE int seek(qint64 pos);
		
		/** property access methods */
		bool seekable();
		qint64 get_duration();
		qint64 get_position();

    public slots:
        void set_position(qint64 position);
        void set_muted(bool muted);
		void set_volume(int volume);
        void pause();
        void play();
        void stop();
    signals:
		void position_changed(qint64 position);
		void media_changed(const QMediaContent &media);
		void volume_changed(int volume);
		void muted_changed(bool muted);
		void playback_state_changed(PlaybackState state);
	private:
        /**
         * The actual player implementation
         */
        QMediaPlayer* backend;

    }; // Player
} // namespace
#endif // _PLAYER_HPP_