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
#include <QMediaPlayer>
#include <QDebug>
#include <QSignalSpy>

#include <gtest/gtest.h>
#include "player.hpp"

using namespace DigitalRooster;

TEST(Player, emitPlay) {
	Player dut;
	QSignalSpy spy(&dut, SIGNAL(playback_state_changed(QMediaPlayer::State state)));

	ASSERT_EQ(spy.count(), 1);
}

