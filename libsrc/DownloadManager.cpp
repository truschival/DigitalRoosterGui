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

#include <DownloadManager.hpp>
#include <QDebug>
#include <appconstants.hpp>

namespace DigitalRooster {

DownloadManager::DownloadManager() {
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
        SLOT(downloadFinished(QNetworkReply*)));
}
/*****************************************************************************/

void DownloadManager::doDownload(const QUrl& url) {
    qDebug() << __FUNCTION__ << "(" << url.toString() <<")";
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

    #if QT_CONFIG(ssl)
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
        SLOT(sslErrors(QList<QSslError>)));
    #endif

    currentDownloads.append(reply);
}
/*****************************************************************************/

QString DownloadManager::saveFileName(const QUrl& url) {
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    QString targetname =
        DigitalRooster::RSS_FILE_DIR + QDir::separator() + basename;

    if (QFile::exists(targetname)) {
        // already exists, don't overwrite
        int i = 0;
        targetname += '.';
        while (QFile::exists(targetname + QString::number(i)))
            ++i;

        targetname += QString::number(i);
    }

    return targetname;
}
/*****************************************************************************/

bool DownloadManager::saveToDisk(const QString& targetpath, QIODevice* data) {
    QFile file(targetpath);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
            qPrintable(targetpath), qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}
/*****************************************************************************/

bool DownloadManager::isHttpRedirect(QNetworkReply* reply) {
    int statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303 ||
        statusCode == 305 || statusCode == 307 || statusCode == 308;
}
/*****************************************************************************/

void DownloadManager::sslErrors(const QList<QSslError>& sslErrors) {
    #if QT_CONFIG(ssl)
    for (const QSslError& error : sslErrors)
        qCritical() << "SSL error:" << qPrintable(error.errorString());
	#else
    Q_UNUSED(sslErrors);
    #endif
}
/*****************************************************************************/

void DownloadManager::downloadFinished(QNetworkReply* reply) {
    QUrl url = reply->url();
    if (reply->error()) {
        qCritical() << "Download of %s failed" <<
            url.toEncoded().constData() << qPrintable(reply->errorString());
    } else {
        if (isHttpRedirect(reply)) {
            qInfo() << "Request redirected";
        } else {
            // TODO: cleanup design for download manager
            emit dataAvailable(reply->readAll());

            QString filename = saveFileName(url);
            // if (saveToDisk(filename, reply)) {
            //    emit newFileAvailable(filename);
            // }
            //emit dataAvailable(reply->readAll());
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();
}

} /* namespace DigitalRooster */
