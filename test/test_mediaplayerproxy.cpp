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
#include <QDebug>
#include <QList>
#include <QMediaPlayer>
#include <QSignalSpy>
#include <QUrl>
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
        : local_audio(
              std::make_shared<DigitalRooster::PodcastEpisode>("TestEpisode",
                  QUrl::fromLocalFile(TEST_FILE_PATH + "/testaudio.mp3")))
        , remote_audio(
              std::make_shared<DigitalRooster::PodcastEpisode>("Remote",
                  QUrl("http://traffic.libsyn.com/armscontrolwonk/138.mp3"))) {
    }

protected:
    std::shared_ptr<PodcastEpisode> local_audio;
    std::shared_ptr<PodcastEpisode> remote_audio;
    const qint64 desired_pos = 10000; // 10 seconds
    MediaPlayerProxy dut;
};

/*****************************************************************************/
TEST_F(PlayerFixture, emitMediaChanged) {
    QSignalSpy spy(&dut, SIGNAL(media_changed(const QMediaContent&)));
    ASSERT_TRUE(spy.isValid());
    dut.set_media(local_audio);
    ASSERT_EQ(spy.count(), 1);
}

/*****************************************************************************/
TEST_F(PlayerFixture, emitStateChanged) {
    QSignalSpy spy(&dut, SIGNAL(playback_state_changed(QMediaPlayer::State)));
    ASSERT_TRUE(spy.isValid());
    dut.set_media(local_audio);
    dut.play();
    spy.wait(500);
    dut.pause();
    ASSERT_EQ(spy.count(), 2); // 1 play, 2 pause
}

/*****************************************************************************/
TEST_F(PlayerFixture, stop) {
    QSignalSpy spy(&dut, SIGNAL(playback_state_changed(QMediaPlayer::State)));
    ASSERT_TRUE(spy.isValid());
    dut.set_media(local_audio);
    dut.play();
    spy.wait(500);
    ASSERT_EQ(dut.playback_state(), QMediaPlayer::PlayingState);
    dut.stop();
    ASSERT_EQ(spy.count(), 2); // 1 play, 2 stop
    ASSERT_EQ(dut.playback_state(), QMediaPlayer::StoppedState);
}

/*****************************************************************************/
TEST_F(PlayerFixture, setMuted) {
    QSignalSpy spy(&dut, SIGNAL(muted_changed(bool)));
    ASSERT_TRUE(spy.isValid());

    dut.set_media(local_audio);
    dut.play();
    dut.set_muted(true);
    dut.pause();
    spy.wait(500);
    ASSERT_TRUE(dut.muted());
    dut.set_muted(false);
    ASSERT_EQ(spy.count(), 2);
    ASSERT_FALSE(dut.muted());
}

/*****************************************************************************/
TEST_F(PlayerFixture, setVolume) {
    QSignalSpy spy(&dut, SIGNAL(volume_changed(int)));
    ASSERT_TRUE(spy.isValid());
    dut.set_volume(23);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(dut.get_volume(), 23);
}

/*****************************************************************************/
TEST_F(PlayerFixture, setVolumeInvalid) {
    QSignalSpy spy(&dut, SIGNAL(volume_changed(int)));
    ASSERT_TRUE(spy.isValid());
 
    dut.set_volume(-1); // invalid
    dut.set_volume(100);

    ASSERT_EQ(spy.count(), 1); // only 1 set calls is valid and should emit 
    ASSERT_EQ(dut.get_volume(), 100);
}

/*****************************************************************************/
TEST_F(PlayerFixture, incrementVolume) {
    QSignalSpy spy(&dut, SIGNAL(volume_changed(int)));
    ASSERT_TRUE(spy.isValid());
    dut.set_volume(23);
    dut.increment_volume(1);
    ASSERT_EQ(spy.count(), 2);
    ASSERT_EQ(dut.get_volume(), 24); // incremented by 1
    dut.increment_volume(-2);
    ASSERT_EQ(dut.get_volume(), 22); // incremented by 2
}

/*****************************************************************************/
TEST_F(PlayerFixture, checkSeekable) {
	QSignalSpy seekspy(&dut, SIGNAL(seekable_changed(bool)));
	ASSERT_TRUE(seekspy.isValid());
    dut.set_media(local_audio);

    dut.play();
    seekspy.wait(1000);
    ASSERT_EQ(seekspy.count(),1);
    ASSERT_TRUE(dut.seekable());

    QSignalSpy spy(&dut, SIGNAL(position_changed(qint64)));
    ASSERT_TRUE(spy.isValid());
    dut.seek(500);
    spy.wait(1000);
    spy.wait(1000);
    ASSERT_GE(spy.count(), 2); // some times position changed was emitted
    ASSERT_GE(dut.get_position(),500); // at least what we seeked
}

/*****************************************************************************/
TEST_F(PlayerFixture, setPositionForward) {
    dut.set_media(local_audio);
    dut.play();

    QSignalSpy spy(&dut, SIGNAL(position_changed(qint64)));
    ASSERT_TRUE(spy.isValid());
    auto pos = dut.get_position();
    dut.set_position(pos+500);
    spy.wait(1000);
    spy.wait(1000);
    ASSERT_GE(spy.count(), 2); // some times position changed was emitted
    ASSERT_GE(dut.get_position(),500); // less than 50ms delta
}

/*****************************************************************************/
TEST_F(PlayerFixture, getDuration) {
   ASSERT_EQ(dut.error(), QMediaPlayer::NoError);
   ASSERT_EQ(dut.media_status(), QMediaPlayer::NoMedia);
   dut.set_media(local_audio);
   // we have to wait until media is loaded
   QSignalSpy spy(
       &dut, SIGNAL(media_status_changed(QMediaPlayer::MediaStatus)));
   ASSERT_TRUE(spy.isValid());
   spy.wait(500);
   EXPECT_EQ(spy.count(), 1);
   auto signal_params = spy.takeFirst();
   EXPECT_EQ(signal_params.at(0).toInt(), QMediaPlayer::LoadedMedia);

   dut.play();
   spy.wait(500);
   EXPECT_EQ(spy.count(), 1);
   signal_params = spy.takeFirst();
   EXPECT_EQ(signal_params.at(0).toInt(), QMediaPlayer::BufferedMedia);

   auto duration = dut.get_duration();
   // Rounded to seconds - it behaves differently on windows
   ASSERT_EQ(duration/1000, 202617/1000);
}

/*****************************************************************************/
TEST_F(PlayerFixture, getStatus) {
   ASSERT_EQ(dut.media_status(), QMediaPlayer::NoMedia);
   dut.set_media(local_audio);
   // we have to wait until media is loaded
   QSignalSpy spy(
       &dut, SIGNAL(media_status_changed(QMediaPlayer::MediaStatus)));
   ASSERT_TRUE(spy.isValid());
   spy.wait(500);
   EXPECT_EQ(spy.count(), 1);
   auto signal_params = spy.takeFirst();
   EXPECT_EQ(signal_params.at(0).toInt(), QMediaPlayer::LoadedMedia);
   EXPECT_EQ(dut.media_status(), QMediaPlayer::LoadedMedia);
}

/*****************************************************************************/
TEST_F(PlayerFixture, checkErrorStates) {
   ASSERT_EQ(dut.error(), QMediaPlayer::NoError);
   ASSERT_EQ(dut.media_status(), QMediaPlayer::NoMedia);
   // No such file
   auto media = std::make_shared<PodcastEpisode>("WebsiteAsEpisode",
       QUrl::fromLocalFile(TEST_FILE_PATH + "/testaudio.mp4"));
   dut.set_media(media);
   // monitor for errors
	QSignalSpy error_spy(&dut, SIGNAL(error(QMediaPlayer::Error)));
   ASSERT_TRUE(error_spy.isValid());

	// we have to wait until media is loaded
   QSignalSpy media_status_spy(
       &dut, SIGNAL(media_status_changed(QMediaPlayer::MediaStatus)));
   ASSERT_TRUE(media_status_spy.isValid());
   media_status_spy.wait(500);
   ASSERT_EQ(media_status_spy.count(), 1);
   auto media_status_events = media_status_spy.takeFirst();
   ASSERT_EQ(
       media_status_events.at(0).toInt(), QMediaPlayer::InvalidMedia);

   dut.play();
	error_spy.wait(500);

   ASSERT_GE(error_spy.count(), 1); // one or more errors
   auto signal_params = error_spy.takeFirst();
   EXPECT_EQ(signal_params.at(0).toInt(), QMediaPlayer::NoMedia);
   EXPECT_EQ(dut.error(), QMediaPlayer::NoMedia);
}
/*****************************************************************************/
TEST_F(PlayerFixture, setPositionRemote) {
    remote_audio->set_position(10000);
    QSignalSpy spy(
        &dut, SIGNAL(media_status_changed(QMediaPlayer::MediaStatus)));
    QSignalSpy spy_playing(
        &dut, SIGNAL(playback_state_changed(QMediaPlayer::State)));
    ASSERT_TRUE(spy.isValid());
    ASSERT_TRUE(spy_playing.isValid());

    dut.set_media(remote_audio);
    spy.wait(200);
    ASSERT_EQ(spy.takeFirst().at(0).toInt(), QMediaPlayer::LoadingMedia);
    spy.wait(500);
    ASSERT_EQ(spy.takeFirst().at(0).toInt(), QMediaPlayer::LoadedMedia);
    dut.set_position(12500);
    dut.play();
    spy_playing.wait(200);
    ASSERT_EQ(
        spy_playing.takeFirst().at(0).toInt(), QMediaPlayer::PlayingState);
    dut.pause();
    spy_playing.wait(200);
    ASSERT_EQ(spy_playing.takeFirst().at(0).toInt(), QMediaPlayer::PausedState);
    EXPECT_GE(dut.get_position(), 12500);
    dut.play();
    spy_playing.wait(200);
    ASSERT_EQ(
        spy_playing.takeFirst().at(0).toInt(), QMediaPlayer::PlayingState);
    EXPECT_GT(dut.get_position(), 12500);
}
/*****************************************************************************/
