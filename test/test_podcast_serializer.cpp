/*******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 ******************************************************************************/
#include <QDateTime>
#include <QSignalSpy>
#include <QUrl>
#include <QUuid>

#include <system_error>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "PodcastSource.hpp"
#include "podcast_serializer.hpp"

using namespace DigitalRooster;
using namespace ::testing;
using ::testing::AtLeast;

/******************************************************************************/
class PodcastSourceMock : public DigitalRooster::PodcastSource {
public:
    PodcastSourceMock()
        : PodcastSource(){

          };

    MOCK_CONST_METHOD0(get_cache_file_impl, QString());
};

/******************************************************************************/
TEST(PodcastSerializer, FileNotFoundThrows) {
    PodcastSerializer dut;
    PodcastSourceMock psmock;
    EXPECT_CALL(psmock, get_cache_file_impl())
        .Times(1)
        .WillOnce(Return(QString("/tmp/some_non_existent_file")));
    ASSERT_THROW(dut.read_from_file(&psmock), std::system_error);
}

/******************************************************************************/
