/******************************************************************************
 * \filename
 * \brief		Hardware abstraction interface functions
 *
 * \details		implementaion is target specific
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
struct BasicInputEvent {
    int code;
    int value;
    int type;
};


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
 * Read scroll_event data from filedescriptor
 * @param filedescriptor of open event file
 */
BasicInputEvent get_input_event(int filedescriptor);

/**
 * Read value of push-button (rotary switch)
 * @param filedescriptor of open event file
 * @return 0/1
 */
int get_pushbutton_value(int filedescriptor);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* _HAL_H_ */
