/*************************************************************************************
 * \filename
 * \brief
 *
 * \details basically the example from
 * 			http://doc.qt.io/qt-5/qtnetwork-download-main-cpp.html
 *
 * \author ruschi
 *
 *************************************************************************************/
#ifndef INCLUDE_DOWNLOADMANAGER_HPP_
#define INCLUDE_DOWNLOADMANAGER_HPP_

#include <QtCore>
#include <QtNetwork>
#include <cstdio>

class QSslError;

namespace DigitalRooster {

class DownloadManager: public QObject {
Q_OBJECT
	QNetworkAccessManager manager;
	QVector<QNetworkReply *> currentDownloads;

public:
	DownloadManager();
	void doDownload(const QUrl &url);
	static QString saveFileName(const QUrl &url);
	bool saveToDisk(const QString &filename, QIODevice *data);
	static bool isHttpRedirect(QNetworkReply *reply);

public slots:
	void downloadFinished(QNetworkReply *reply);
	void sslErrors(const QList<QSslError> &errors);

signals:
	void newFileAvailable(const QString &filename);

};

} /* namespace DigitalRooster */

#endif /* INCLUDE_DOWNLOADMANAGER_HPP_ */
