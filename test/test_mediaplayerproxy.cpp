// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QDebug>
#include <QMediaPlayer>
#include <QSignalSpy>
#include <QUrl>
#include <QVariant>

#include <gtest/gtest.h>
#include <memory>

#include "PlayableItem.hpp"
#include "appconstants.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

class PlayerFixture : public virtual ::testing::Test {
public:
    PlayerFixture()
        : local_audio(
              std::make_shared<DigitalRooster::PodcastEpisode>("TestEpisode",
                  QUrl::fromLocalFile(TEST_FILE_PATH + "/testaudio.mp3")))
        , remote_audio(
              std::make_shared<DigitalRooster::PodcastEpisode>("Remote",
                  QUrl("https://github.com/truschival/DigitalRoosterGui/raw/"
                       "develop/test/testaudio.mp3"))) {
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
    QSignalSpy spy(&dut, SIGNAL(volume_changed(double)));
    ASSERT_TRUE(spy.isValid());
    dut.set_volume(23.0);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_FLOAT_EQ(dut.get_volume(), 23.0);
}

/*****************************************************************************/
TEST_F(PlayerFixture, setVolumeInvalid) {
    QSignalSpy spy(&dut, SIGNAL(volume_changed(double)));
    ASSERT_TRUE(spy.isValid());

    dut.set_volume(-1); // invalid
    dut.set_volume(100);

    ASSERT_EQ(spy.count(), 2);
    // lower bound clamp
    auto arguments = spy.takeFirst();
    EXPECT_DOUBLE_EQ(arguments.at(0).toDouble(), 0.0);
    arguments = spy.takeFirst();
    EXPECT_DOUBLE_EQ(arguments.at(0).toDouble(), 100.0);

    ASSERT_EQ(dut.get_volume(), 100);
}

/*****************************************************************************/
TEST_F(PlayerFixture, incrementVolume) {
    QSignalSpy spy(&dut, SIGNAL(volume_changed(double)));
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
    ASSERT_EQ(seekspy.count(), 1);
    ASSERT_TRUE(dut.seekable());

    QSignalSpy spy(&dut, SIGNAL(position_changed(qint64)));
    ASSERT_TRUE(spy.isValid());
    dut.seek(500);
    spy.wait(1000);
    spy.wait(1000);
    ASSERT_GE(spy.count(), 2); // some times position changed was emitted
    ASSERT_GE(dut.get_position(), 500); // at least what we seeked
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
    ASSERT_EQ(duration / 1000, 202617 / 1000);
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
    ASSERT_EQ(media_status_events.at(0).toInt(), QMediaPlayer::InvalidMedia);

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
    QSignalSpy spy_status(
        &dut, SIGNAL(media_status_changed(QMediaPlayer::MediaStatus)));
    QSignalSpy spy_playing(
        &dut, SIGNAL(playback_state_changed(QMediaPlayer::State)));
    QSignalSpy spy_seekable(&dut, SIGNAL(seekable_changed(bool)));
    ASSERT_TRUE(spy_status.isValid());
    ASSERT_TRUE(spy_seekable.isValid());
    ASSERT_TRUE(spy_playing.isValid());

    dut.set_media(remote_audio);
    ASSERT_FALSE(remote_audio->is_seekable()); // not seekable
    /* normal media-loading cycle: loading->loaded */
    spy_status.wait(600);
    ASSERT_FALSE(spy_status.isEmpty());
    ASSERT_EQ(spy_status.takeFirst().at(0).toInt(), QMediaPlayer::LoadingMedia);
    spy_status.wait(1000);
    ASSERT_FALSE(spy_status.isEmpty());
    ASSERT_EQ(spy_status.takeFirst().at(0).toInt(), QMediaPlayer::LoadedMedia);
    /* now start playing */
    dut.play();
    spy_playing.wait(200);
    ASSERT_EQ(
        spy_playing.takeFirst().at(0).toInt(), QMediaPlayer::PlayingState);
    spy_seekable.wait(100);
    spy_seekable.wait(400);
    ASSERT_GE(spy_seekable.count(), 1);
    ASSERT_TRUE(remote_audio->is_seekable()); // playing, should be seekable
    dut.pause();
    spy_playing.wait(100);
    /* Media position should not have been reset to < 10000 when media was not
     * available */
    EXPECT_GE(remote_audio->get_position(), 10000);
    dut.stop();
    /* Media position should not have been reset to < 10000 when media was not
     * available */
    EXPECT_GE(remote_audio->get_position(), 10000);
}
/*****************************************************************************/
