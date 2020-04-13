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
#include <QLoggingCategory>

#include <vector>

#include "appconstants.hpp"
#include "httpclient.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HttpClient");

/*****************************************************************************/

HttpClient::HttpClient() {
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
        SLOT(downloadFinished(QNetworkReply*)));
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}
/*****************************************************************************/

void HttpClient::doDownload(const QUrl& url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << "(" << url.toString() << ")";
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

#if QT_CONFIG(ssl)
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
        SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloads.push_back(reply);
}
/*****************************************************************************/

bool HttpClient::isHttpRedirect(QNetworkReply* reply) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    int statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303 ||
        statusCode == 305 || statusCode == 307 || statusCode == 308;
}
/*****************************************************************************/

void HttpClient::sslErrors(const QList<QSslError>& sslErrors) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
#if QT_CONFIG(ssl)
    for (const QSslError& error : sslErrors)
        qCCritical(CLASS_LC) << "SSL error:" << qPrintable(error.errorString());
#else
    Q_UNUSED(sslErrors);
#endif
}
/*****************************************************************************/

void HttpClient::downloadFinished(QNetworkReply* reply) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QUrl url = reply->url();
    if (reply->error()) {
        qCCritical(CLASS_LC)
            << "Download failed" << url.toEncoded().constData()
            << qPrintable(reply->errorString());
    } else {
        if (isHttpRedirect(reply)) {
            qCInfo(CLASS_LC) << "Request redirected";
        } else {
            emit dataAvailable(reply->readAll());
        }
    }

    auto end_it = std::remove(currentDownloads.begin(), currentDownloads.end(), reply);
    currentDownloads.erase(end_it,currentDownloads.end());
    reply->deleteLater();
}
