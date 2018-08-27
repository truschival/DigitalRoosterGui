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

#include <QDebug>
#include <appconstants.hpp>
#include <httpclient.hpp>

namespace DigitalRooster {

HttpClient::HttpClient() {
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
        SLOT(downloadFinished(QNetworkReply*)));
}
/*****************************************************************************/

void HttpClient::doDownload(const QUrl& url) {
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

bool HttpClient::isHttpRedirect(QNetworkReply* reply) {
    int statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303 ||
        statusCode == 305 || statusCode == 307 || statusCode == 308;
}
/*****************************************************************************/

void HttpClient::sslErrors(const QList<QSslError>& sslErrors) {
    #if QT_CONFIG(ssl)
    for (const QSslError& error : sslErrors)
        qCritical() << "SSL error:" << qPrintable(error.errorString());
	#else
    Q_UNUSED(sslErrors);
    #endif
}
/*****************************************************************************/

void HttpClient::downloadFinished(QNetworkReply* reply) {
    QUrl url = reply->url();
    if (reply->error()) {
        qCritical() << "Download of %s failed" <<
            url.toEncoded().constData() << qPrintable(reply->errorString());
    } else {
        if (isHttpRedirect(reply)) {
            qInfo() << "Request redirected";
        } else {
            emit dataAvailable(reply->readAll());
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();
}

} /* namespace DigitalRooster */
