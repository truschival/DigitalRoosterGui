/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include "mediaplayerproxy.hpp"
#include "PlayableItem.hpp"
#include <QAudio>
#include <QDebug>
#include <QLoggingCategory>
#include <QMediaPlayer>

#include <QMediaMetaData>
using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.MediaPlayerProxy");

/***********************************************************************/

MediaPlayerProxy::MediaPlayerProxy()
    : backend(std::make_unique<QMediaPlayer>()) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QObject::connect(backend.get(), &QMediaPlayer::mediaChanged,
        [=](const QMediaContent& media) { emit media_changed(media); });

    QObject::connect(
        backend.get(), &QMediaPlayer::positionChanged, [=](qint64 position) {
            emit position_changed(position);
            if (current_item.get() != nullptr) {
                current_item->set_position(position);
            }
        });

    QObject::connect(backend.get(),
        static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(
            &QMediaPlayer::error),
        [=](QMediaPlayer::Error err) {
            qCWarning(CLASS_LC) << "Player Error" << err;
            emit error(err);
        });

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

    QObject::connect(backend.get(), &QMediaPlayer::metaDataAvailableChanged,
        [=](bool available) {
            qCDebug(CLASS_LC) << " metaDataAvailableChanged " << available;
            if (available) {
                QString title =
                    backend->metaData(QMediaMetaData::Title).toString();
                QString publisher =
                    backend->metaData(QMediaMetaData::Publisher).toString();

                qCDebug(CLASS_LC)
                    << "\n\tTitle:" << title << "\n\tPublisher:" << publisher
                    << "\n\tAlbumArtist:"
                    << backend->metaData(QMediaMetaData::AlbumArtist).toString()
                    << "\n\tAuthor:"
                    << backend->metaData(QMediaMetaData::Author).toString()
                    << "\n\tDescription:"
                    << backend->metaData(QMediaMetaData::Description)
                           .toString();

                if (title != "") {
                    current_item->set_title(title);
                }
                if (publisher != "") {
                    current_item->set_publisher(publisher);
                }
            } else {
                qCDebug(CLASS_LC) << "No metadata.";
            }
        });
}

/*****************************************************************************/
void MediaPlayerProxy::do_seek(qint64 incr) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (seekable())
        set_position(get_position() + incr);
};

/*****************************************************************************/
bool MediaPlayerProxy::is_seekable() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->isSeekable();
}

/*****************************************************************************/
QMediaPlayer::Error MediaPlayerProxy::do_error() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->error();
}

/*****************************************************************************/
qint64 MediaPlayerProxy::do_get_position() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->position();
}

/*****************************************************************************/

QMediaPlayer::MediaStatus MediaPlayerProxy::do_media_status() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->mediaStatus();
}

/*****************************************************************************/
qint64 MediaPlayerProxy::do_get_duration() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->duration();
}

/*****************************************************************************/
void MediaPlayerProxy::do_set_position(qint64 position) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << position;
    backend->setPosition(position);
}
/*****************************************************************************/
bool MediaPlayerProxy::is_muted() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->isMuted();
}

/*****************************************************************************/
int MediaPlayerProxy::do_get_volume() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return linear_volume;
}

/*****************************************************************************/
void MediaPlayerProxy::do_set_media(
    std::shared_ptr<DigitalRooster::PlayableItem> media) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << media->get_display_name();
    current_item = media;
    auto previous_position = media->get_position();

    backend->setMedia(QMediaContent(media->get_url()));
    if (previous_position != 0) {
        qCDebug(CLASS_LC) << "restarting from position" << previous_position;
        set_position(previous_position);
    }
}
/*****************************************************************************/
void MediaPlayerProxy::do_set_playlist(QMediaPlaylist* playlist) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    backend->setPlaylist(playlist);
}
/*****************************************************************************/
void MediaPlayerProxy::do_set_muted(bool muted) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    backend->setMuted(muted);
}

/*****************************************************************************/
QMediaPlayer::State MediaPlayerProxy::do_playback_state() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return backend->state();
}

/*****************************************************************************/
void MediaPlayerProxy::do_set_volume(int volume) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << volume;
    if (volume < 0 || volume > 100) {
        qCWarning(CLASS_LC) << "invalid volume (must be 0..100%)";
        return;
    }
    linear_volume = volume;
    emit volume_changed(volume);

    /* backend works with logarithmic volume */
    auto adjusted_volume = QAudio::convertVolume(volume / qreal(100.0),
        QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    backend->setVolume(qRound(adjusted_volume * 100.0));
}

/*****************************************************************************/
void MediaPlayerProxy::do_increment_volume(int increment) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << increment;
    qCDebug(CLASS_LC) << " current volume" << linear_volume;
    set_volume(linear_volume + increment);
}

/*****************************************************************************/
void MediaPlayerProxy::do_pause() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    backend->pause();
}
/*****************************************************************************/
void MediaPlayerProxy::do_play() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    backend->play();
}
/*****************************************************************************/
void MediaPlayerProxy::do_stop() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    backend->stop();
}
/*****************************************************************************/
