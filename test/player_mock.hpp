/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include "gmock/gmock.h"
#include "mediaplayer.hpp"
#include "gtest/gtest.h"

class PlayerMock : public DigitalRooster::MediaPlayer {
public:
    PlayerMock(){

    };
    MOCK_METHOD0(do_play, void());
    MOCK_METHOD0(do_stop, void());
    MOCK_METHOD0(do_pause, void());

    MOCK_CONST_METHOD0(is_seekable, bool());
    MOCK_CONST_METHOD0(is_muted, bool());
    MOCK_CONST_METHOD0(do_get_volume, int());
    MOCK_CONST_METHOD0(do_get_duration, qint64());
    MOCK_CONST_METHOD0(do_get_position, qint64());
    MOCK_CONST_METHOD0(do_media_status, QMediaPlayer::MediaStatus());
    MOCK_CONST_METHOD0(do_playback_state, QMediaPlayer::State());
    MOCK_CONST_METHOD0(do_error, QMediaPlayer::Error());

    MOCK_METHOD1(do_set_media,
        void(std::shared_ptr<DigitalRooster::PlayableItem> media));
    MOCK_METHOD1(do_set_playlist, void(QMediaPlaylist* playlist));

    MOCK_METHOD1(do_set_position, void(qint64 position));
    MOCK_METHOD1(do_set_muted, void(bool muted));
    MOCK_METHOD1(do_set_volume, void(int volume));
    MOCK_METHOD1(do_seek, void(qint64 position));
    MOCK_METHOD1(do_increment_volume, void(int increment));

    void emitError(QMediaPlayer::Error err) {
        emit error(err);
    }
};
