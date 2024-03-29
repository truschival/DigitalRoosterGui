/******************************************************************************
 * \filename
 * \brief
 *
 * \details basically the example from
 * 			http://doc.qt.io/qt-5/qtnetwork-download-main-cpp.html
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _HTTPCLIENT_HPP_
#define _HTTPCLIENT_HPP_

#include <QObject>
#include <QByteArray>
#include <QtCore>
#include <QtNetwork>
#include <QNetworkAccessManager>

class QSslError;
class QNetworkReply;

namespace DigitalRooster {

/**
 * Simple client for HTTP downloads basically the example from
 * http://doc.qt.io/qt-5/qtnetwork-download-main-cpp.html
 */
class HttpClient : public QObject {
    Q_OBJECT
    QNetworkAccessManager manager;
    std::vector<QNetworkReply*> pending_downloads;

public:
    HttpClient();
    void doDownload(const QUrl& url);
    static bool isHttpRedirect(QNetworkReply* reply);

public slots:
    void downloadFinished(QNetworkReply* reply);
    void sslErrors(const QList<QSslError>& errors);

signals:
    void dataAvailable(QByteArray content);

};

} /* namespace DigitalRooster */

#endif /* _HTTPCLIENT_HPP_ */
