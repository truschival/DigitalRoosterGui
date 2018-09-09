/******************************************************************************
 * \filename
 * \brief    controls brightness
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <QAudio>
#include <QDebug>
#include <QLoggingCategory>
#include "hwif/hal.h"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.BrightnessControl");

/*****************************************************************************/

BrightnessControl::BrightnessControl()
    : linear_brightness(0)
    , log_brightness(1) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    set_brightness(30);
}

/*****************************************************************************/
void BrightnessControl::set_brightness(int brightness){
    qCDebug(CLASS_LC) << Q_FUNC_INFO << "Linear: " <<brightness;

    auto linearVolume = QAudio::convertVolume(brightness / qreal(100.0),
        QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    qCDebug(CLASS_LC) << " = Logarithmic:" << qRound(linearVolume * 100);

    ::set_brightness(qRound(linearVolume * 100));
}
/*****************************************************************************/



