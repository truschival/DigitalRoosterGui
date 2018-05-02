/******************************************************************************
* \filename
* \brief   Interface for an abstract mediaplayer used to mock MediaPlayerProxy
*
* \details
*
* \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
* \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
* 			 SPDX-License-Identifier: GPL-2.0-or-later}
*
*****************************************************************************/

#ifndef _MEDIAPLAYER_HPP_
#define _MEDIAPLAYER_HPP_

#include <QMediaPlayer>
#include <QObject>
#include <memory>

namespace DigitalRooster {
	class PlayableItem;
	class PodcastEpisode;

	/**
	 * Interface class with NVP methods to Mock MediaPlayerProxy behavior
	 */
    class MediaPlayer : public QObject {
        Q_OBJECT
        Q_PROPERTY(qint64 position READ get_position NOTIFY position_changed)
        Q_PROPERTY(qint64 duration READ get_duration NOTIFY duration_changed)
        Q_PROPERTY(
            int volume READ get_volume WRITE set_volume NOTIFY volume_changed)
        Q_PROPERTY(bool muted READ muted WRITE set_muted NOTIFY muted_changed)
        Q_PROPERTY(bool seekable READ seekable NOTIFY seekable_changed)
        Q_PROPERTY(QMediaPlayer::MediaStatus mediaStatus READ media_status
                NOTIFY media_status_changed)
        Q_PROPERTY(QMediaPlayer::State playbackState READ playback_state NOTIFY
                playback_state_changed)
    public:
        MediaPlayer(const MediaPlayer& rhs) = delete;
        MediaPlayer& operator =(const MediaPlayer & rhs) = delete;
		virtual ~MediaPlayer()=default;

		bool muted() const;
		int get_volume() const;
		void set_muted(bool muted);
		void set_volume(int volume);

		bool seekable() const;
		Q_INVOKABLE void seek(qint64 incr);

		qint64 get_position() const;
		void set_position(qint64 position);

		qint64 get_duration() const;

		QMediaPlayer::MediaStatus media_status() const;
		QMediaPlayer::State playback_state() const;
		QMediaPlayer::Error error() const;

	public slots:
        void set_media(std::shared_ptr<DigitalRooster::PlayableItem> media);
        void set_playlist(QMediaPlaylist* playlist);

        void pause();
        void play();
        void stop();
    signals:
        void position_changed(qint64 position);
        void duration_changed(qint64 duration);
        void media_changed(const QMediaContent& media);
        void volume_changed(int volume);
        void muted_changed(bool muted);
        void seekable_changed(bool seekable);
        void media_status_changed(QMediaPlayer::MediaStatus);
        void playback_state_changed(QMediaPlayer::State state);
        void error(QMediaPlayer::Error error);

    protected:
        MediaPlayer() = default;

    private:
		/*****************************************************************************
		 Virtual Function are only implemented because QML does not want to register
		 abstract class as type using

		 qmlRegisterType<DigitalRooster::MediaPlayer>("ruschi.MediaPlayer", 1, 0,
				"MediaPlayer");
		******************************************************************************/
		virtual bool is_seekable() const;
		virtual bool is_muted() const;
		virtual int do_get_volume()const;
		virtual qint64 do_get_duration() const;
		virtual qint64 do_get_position() const;
		virtual QMediaPlayer::MediaStatus do_media_status() const;
		virtual QMediaPlayer::State do_playback_state()const;
		virtual QMediaPlayer::Error do_error() const;
		virtual void do_set_media(std::shared_ptr<DigitalRooster::PlayableItem> media) {};
		virtual void do_set_playlist(QMediaPlaylist* playlist) {};
		virtual void do_set_position(qint64 position) {};
		virtual void do_set_muted(bool muted) {};
		virtual void do_set_volume(int volume) {};
		virtual void do_seek(qint64 incr) {};
		virtual void do_pause() {};
		virtual void do_play() {};
		virtual void do_stop() {};
	};
}
#endif /*_MEDIAPLAYER_HPP_*/