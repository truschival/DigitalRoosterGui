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
#include <QDebug>
#include <QMediaPlayer>
#include <QSignalSpy>
#include <QUrl>
#include <memory>

#include <gtest/gtest.h>

#include "PlayableItem.hpp"
#include "appconstants.hpp"
#include "player.hpp"

using namespace DigitalRooster;

class PlayerFixture : public virtual ::testing::Test {
public:
    PlayerFixture()
        : podcast(std::make_unique<PodcastEpisode>("TestEpisode",
              QUrl::fromLocalFile(TEST_FILE_PATH + "/testaudio.mp3"))) {
        qDebug() << TEST_FILE_PATH + "/testaudio.mp3";
    }


protected:
    std::unique_ptr<PodcastEpisode> podcast;
    Player dut;
};
/*****************************************************************************/
TEST_F(PlayerFixture, emitMediaChanged) {
    QSignalSpy spy(&dut, SIGNAL(media_changed(const QMediaContent&)));
    ASSERT_TRUE(spy.isValid());
    dut.set_media(podcast.get());
    ASSERT_EQ(spy.count(), 1);
}


/*****************************************************************************/
TEST_F(PlayerFixture, emitStateChanged) {
    QSignalSpy spy(&dut, SIGNAL(playback_state_changed(QMediaPlayer::State)));
    ASSERT_TRUE(spy.isValid());
    dut.set_media(podcast.get());
    dut.play();
    spy.wait();
    dut.pause();
    ASSERT_EQ(spy.count(), 2); // 1 play, 2 pause
}
