/******************************************************************************
 * \filename
 * \brief		Hardware abstraction interface functions
 *
 * \details		implementation is target specific
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _HAL_H_
#define _HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Simplified linux/input event to allow portable code
 */
struct InputEvent {
    int code;
    int value;
    int type;
};

#define EVT_TYPE_KEY_PRESSED 1
#define EVT_TYPE_KEY_RELEASED 0

/**
 * Hardware init
 * @return
 */

int setup_hardware();

/**
 * Setup GPIO pin for push_button
 * @return filehandle for pushbutton
 */
int get_push_button_handle();

/**
 * Setup rotary button file event interface
 * @return filehandle for event file
 */
int get_rotary_button_handle();

/**
 * Power management functions
 */
int system_reboot();
int system_poweroff();

/**
 * Set brightness of display on target
 * @param brightness 0..100 %
 */
int set_brightness(int brightness);

/**
 * Read input-event data from filedescriptor
 * @param filedescriptor of open event file
 */
InputEvent get_input_event(int filedescriptor);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* _HAL_H_ */
