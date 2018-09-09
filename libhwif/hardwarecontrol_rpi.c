/******************************************************************************
 * \filename
 * \brief    Raspberry PI specific hardware interface functions
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>

#include <wiringPi.h>

/*****************************************************************************/
int system_reboot() {
    sync();
    return reboot(LINUX_REBOOT_CMD_RESTART);
}

/*****************************************************************************/

int system_poweroff(){
	sync();
	return reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

/*****************************************************************************/
int set_brightness(int brightness){
	pwmWrite(13, brightness);
	return 0;
}

/*****************************************************************************/
int setup_hardware(){
	wiringPiSetup();
	pinMode(13,PWM_OUTPUT) ;
	pwmSetClock(1000);
	pwmWrite(13, 10);
	return 0;
}
