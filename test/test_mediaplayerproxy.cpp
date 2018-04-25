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
#include <QList>
#include <QVariant>
#include <memory>

#include <gtest/gtest.h>
#include <mediaplayerproxy.hpp>

#include "PlayableItem.hpp"
#include "appconstants.hpp"

using namespace DigitalRooster;

class PlayerFixture : public virtual ::testing::Test {
public:
    PlayerFixture()
        : podcast(std::make_unique<DigitalRooster::PodcastEpisode>("TestEpisode",
              QUrl::fromLocalFile(TEST_FILE_PATH + "/testaudio.mp3"))) {
    }

protected:
    std::unique_ptr<PodcastEpisode> podcast;
    const qint64 desired_pos = 10000; //10 seconds
    MediaPlayerProxy dut;
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

/*****************************************************************************/
TEST_F(PlayerFixture, setPositionEmitsPositionChanged) {
    QSignalSpy spy(&dut, SIGNAL(position_changed(qint64)));
    ASSERT_TRUE(spy.isValid());
    dut.set_media(podcast.get());
    ASSERT_TRUE(dut.seekable());
    dut.set_position(desired_pos);
    ASSERT_GT(spy.count(), 1);
}

/*****************************************************************************/
TEST_F(PlayerFixture, seekEmitsPositionChangedToNewPosition) {
    QSignalSpy spy(&dut, SIGNAL(position_changed(qint64)));
    ASSERT_TRUE(spy.isValid());

    dut.set_media(podcast.get());
    dut.play();
	spy.wait();
	auto pos = dut.get_position();
    dut.seek(5000); // forward 5s

    ASSERT_GT(spy.count(), 1);

    QList<QVariant> arguments = spy.takeLast();
    bool ok;
    auto read_pos = arguments.at(0).toLongLong(&ok);
    ASSERT_TRUE(ok);
    ASSERT_GT(read_pos,pos+5000);
}


/*****************************************************************************/
TEST_F(PlayerFixture, setMuted) {
	QSignalSpy spy(&dut, SIGNAL(muted_changed(bool)));
	ASSERT_TRUE(spy.isValid());

	dut.set_media(podcast.get());
	dut.play();
	dut.set_muted(true);
	dut.pause();
	spy.wait();
	ASSERT_TRUE(dut.muted());
	dut.set_muted(false);
	ASSERT_EQ(spy.count(), 2);
	ASSERT_FALSE(dut.muted());
}


/*****************************************************************************/
TEST_F(PlayerFixture, setVolume) {
	QSignalSpy spy(&dut, SIGNAL(volume_changed(int)));
	ASSERT_TRUE(spy.isValid());

	dut.set_media(podcast.get());
	dut.play();
	dut.set_volume(50);
	ASSERT_EQ(spy.count(), 1);
	ASSERT_EQ(dut.get_volume(),50);
}
