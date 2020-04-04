/******************************************************************************
 * \filename
 * \brief	common test functions
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#ifndef TEST_COMMON_HPP_
#define TEST_COMMON_HPP_



/**
 * file name for debug output during test run
 */
#ifndef TEST_LOG_NAME
#define TEST_LOG_NAME tests.log
#endif

void setup_test_logs();

#endif /* TEST_COMMON_HPP_ */
