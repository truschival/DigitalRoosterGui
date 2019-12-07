/*******************************************************************************
 * \filename
 * \brief	Allows mocking of the wallclock during test
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 ******************************************************************************/

#ifndef TEST_SERIALIZER_MOCK_HPP_
#define TEST_SERIALIZER_MOCK_HPP_
#include <chrono>
#include <gmock/gmock.h>
#include <QDir>

#include "podcast_serializer.hpp"

/******************************************************************************/
class SerializerMock : public DigitalRooster::PodcastSerializer {
public:
    SerializerMock(const QDir& app_cache_dir,
        DigitalRooster::PodcastSource* source = nullptr,
        std::chrono::milliseconds delay = std::chrono::milliseconds(100))
        : PodcastSerializer(app_cache_dir, source, delay){};
    MOCK_METHOD0(write_cache, void());
    MOCK_METHOD0(delete_cache, void());
};


/*****************************************************************************/
#endif /* TEST_SERIALIZER_MOCK_HPP_ */
