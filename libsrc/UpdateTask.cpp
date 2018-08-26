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

#include <QCryptographicHash>
#include <cstdio>
#include <stdexcept>

#include "DownloadManager.hpp"
#include "PodcastSource.hpp"
#include "PodcastSourceReader.hpp"
#include "UpdateTask.hpp"

namespace DigitalRooster {

UpdateTask::UpdateTask(PodcastSource& source) :
		ps(source) {
	connect(&dlm, SIGNAL(newFileAvailable(const QString&)), this,
			SLOT(newFileAvailable(const QString&)));

    connect(&dlm, SIGNAL(dataAvailable(const QByteArray)), this,
        SLOT(dataAvailable(const QByteArray)));
}
/*****************************************************************************/

void UpdateTask::newFileAvailable(const QString& filepath) {
	QFile file(filepath);
	QCryptographicHash sha256(QCryptographicHash::Sha256);

	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		sha256.addData(file.readAll());
		if (lasthash != sha256.result()) {
			lasthash = sha256.result();
			//qDebug() << "SHA256 :" << sha256.result().toHex();
			ps.set_rss_file(filepath);
			update_podcast(ps);
			emit newDataAvailable();
		}
		//else nothing new to do
	} else {
		throw std::system_error(
				make_error_code(std::errc::no_such_file_or_directory),
				file.errorString().toStdString());
	}
	file.remove();
}

/*****************************************************************************/

void UpdateTask::dataAvailable(const QByteArray& data) {
       update_podcast(ps, data);
}

/*****************************************************************************/

void UpdateTask::start() {
	dlm.doDownload(ps.get_url());
}

} /* namespace DigitalRooster */
