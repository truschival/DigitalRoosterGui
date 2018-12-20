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

#include <QLoggingCategory>

#include "configuration_manager.hpp"
#include "volume_button.hpp"

using namespace DigitalRooster;
static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.VolumeButton");

using namespace DigitalRooster;


/*****************************************************************************/
DigitalRooster::VolumeButton::VolumeButton(
    DigitalRooster::ConfigurationManager* cm, QString rotary_encoder,
    QString button, QObject* parent)
    : rotary_file(rotary_encoder)
    , button_file(button)
    , volume(cm->get_volume()) {

    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    /* connect notifier and handler for  rotary encoder */
    if (rotary_file.open(QFile::ReadOnly)) {
        rotary_notifier = std::make_unique<QSocketNotifier>(
            rotary_file.handle(), QSocketNotifier::Read);
        rotary_notifier->setEnabled(true);
        connect(rotary_notifier.get(), SIGNAL(activated(int)), this,
            SLOT(read_rotary));

    } else {
        qCCritical(CLASS_LC) << " open button file " << button << "failed!";
    }

	/* connect notifier and handler for push button */
    if (button_file.open(QFile::ReadOnly)) {
        button_notifier = std::make_unique<QSocketNotifier>(
            button_file.handle(), QSocketNotifier::Read);
        button_notifier->setEnabled(true);
        connect(button_notifier.get(), SIGNAL(activated(int)), this,
            SLOT(read_button));
    } else {
        qCCritical(CLASS_LC) << " open button file " << button << "failed!";
    }
}

/*****************************************************************************/
VolumeButton::~VolumeButton() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
	if (rotary_file.isOpen())
        rotary_file.close();

    if (button_file.isOpen())
        button_file.close();
}

/*****************************************************************************/
int DigitalRooster::VolumeButton::read_rotary() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    emit volume_changed(volume);
    return 0;
}

/*****************************************************************************/
int DigitalRooster::VolumeButton::read_button() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    emit button_pressed();
    return 0;
}

/*****************************************************************************/
int DigitalRooster::VolumeButton::get_volume() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return volume;
}

/*****************************************************************************/
bool DigitalRooster::VolumeButton::get_button_state() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return button_state;
}

/*****************************************************************************/