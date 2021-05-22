// SPDX-License-oiIdentifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include "gmock/gmock.h"
#include "mediaplayer.hpp"
#include "gtest/gtest.h"

class PlayerMock : public DigitalRooster::MediaPlayer {
public:
    PlayerMock(){

    };
    MOCK_METHOD(void, do_play, ());
    MOCK_METHOD(void, do_stop, ());
    MOCK_METHOD(void, do_pause, ());

    MOCK_METHOD(bool, is_seekable, (), (const));
    MOCK_METHOD(bool, is_muted, (), (const));
    MOCK_METHOD(double, do_get_volume, (), (const));
    MOCK_METHOD(qint64, do_get_duration, (), (const));
    MOCK_METHOD(qint64, do_get_position, (), (const));

    MOCK_METHOD(QMediaPlayer::MediaStatus, do_media_status, (), (const));
    MOCK_METHOD(QMediaPlayer::State, do_playback_state, (), (const));
    MOCK_METHOD(QMediaPlayer::Error, do_error, (), (const));

    MOCK_METHOD(void, do_set_media,
        (std::shared_ptr<DigitalRooster::PlayableItem>), (override));

    MOCK_METHOD(void, do_set_playlist, (QMediaPlaylist*), (override));

    MOCK_METHOD(void, do_set_position, (qint64), (override));
    MOCK_METHOD(void, do_seek, (qint64), (override));

    MOCK_METHOD(void, do_set_muted, (bool), (override));
    MOCK_METHOD(void, do_set_volume, (double), (override));
    MOCK_METHOD(void, do_increment_volume, (double), (override));

    void emitError(QMediaPlayer::Error err) {
        emit error(err);
    }
};
