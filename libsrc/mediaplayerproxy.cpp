// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>
#include <QMediaMetaData>
#include <QMediaPlayer>

#include "PlayableItem.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.MediaPlayerProxy");

/****************************************************************************/
MediaPlayerProxy::MediaPlayerProxy()
    : backend(std::make_unique<QMediaPlayer>()) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QObject::connect(backend.get(), &QMediaPlayer::mediaChanged,
        [=](const QMediaContent& media) {
            qCDebug(CLASS_LC) << "MediaPlayerProxy media_changed()";
            /*
             * We know nothing of new media, assume it is not seekable
             * otherwise we might overwrite/lose saved position
             */
            current_item->set_seekable(false);
            emit media_changed(media);
        });

    QObject::connect(
        backend.get(), &QMediaPlayer::positionChanged, [=](qint64 position) {
            qCDebug(CLASS_LC)
                << "MediaPlayerProxy position_changed()" << position;
            emit position_changed(position);

            /** Only update position of seekable media */
            if (current_item && position_updateable) {
                current_item->set_position(position);
            }
        });

    QObject::connect(backend.get(),
        static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(
            &QMediaPlayer::error),
        [=](QMediaPlayer::Error err) {
            qCWarning(CLASS_LC) << "MediaPlayerProxy Error" << err;
            emit error(err);
        });

    QObject::connect(backend.get(), &QMediaPlayer::mutedChanged,
        [=](bool muted) { emit muted_changed(muted); });

    QObject::connect(backend.get(), &QMediaPlayer::stateChanged,
        [=](QMediaPlayer::State state) {
            qCDebug(CLASS_LC)
                << "MediaPlayerProxy playback_state_changed()" << state;

            emit playback_state_changed(state);
        });

    QObject::connect(backend.get(), &QMediaPlayer::durationChanged,
        [=](qint64 duration) { emit duration_changed(duration); });

    QObject::connect(
        backend.get(), &QMediaPlayer::seekableChanged, [=](bool seekable) {
            qCDebug(CLASS_LC)
                << "MediaPlayerProxy seekable_changed()" << seekable;
            current_item->set_seekable(seekable);
            enable_position_update(seekable);
            emit seekable_changed(seekable);

            /* jump to previously saved position once we know we can seek */
            if (seekable) {
                /* update backend position */
                set_position(current_item->get_position());
            }
        });

    QObject::connect(backend.get(), &QMediaPlayer::mediaStatusChanged,
        [=](QMediaPlayer::MediaStatus status) {
            qCDebug(CLASS_LC)
                << "MediaPlayerProxy media_status_changed()" << status;
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
                    << "\n\tPublisher:" << publisher << "\n\tAlbumArtist:"
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
    if (seekable()) {
        set_position(get_position() + incr);
    }
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
void MediaPlayerProxy::enable_position_update(bool enable) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << enable;
    position_updateable = enable;
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
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
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
    /* make sure we don't reset current position to 0 on stop
     * as QMediaPlayer usually does */
    enable_position_update(false);
    backend->stop();
}
/*****************************************************************************/
