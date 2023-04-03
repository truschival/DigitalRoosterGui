// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QLoggingCategory>
#include <vector>

#include "appconstants.hpp"
#include "httpclient.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.HttpClient");

/*****************************************************************************/
HttpClient::HttpClient() {
    connect(&manager, &QNetworkAccessManager::finished, this,
        &HttpClient::downloadFinished);
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
void HttpClient::doDownload(const QUrl& url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << "(" << url.toString() << ")";
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply* reply = manager.get(request);

#if QT_CONFIG(ssl)
    connect(reply, &QNetworkReply::sslErrors, this, &HttpClient::sslErrors);
#endif

    pending_downloads.push_back(reply);
}

/*****************************************************************************/
void HttpClient::
    sslErrors(const QList<QSslError>& sslErrors) {
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
        qCCritical(CLASS_LC) << "Download failed" << url.toEncoded().constData()
                             << qPrintable(reply->errorString());
    } else {
        emit dataAvailable(reply->readAll());
    }

    auto end_it =
        std::remove(pending_downloads.begin(), pending_downloads.end(), reply);
    pending_downloads.erase(end_it, pending_downloads.end());
    reply->deleteLater();
}
