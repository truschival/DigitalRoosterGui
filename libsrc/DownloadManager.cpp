/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#include <appconstants.hpp>
#include <DownloadManager.hpp>
#include <QDebug>

namespace DigitalRooster {

DownloadManager::DownloadManager() {
	connect(&manager, SIGNAL(finished(QNetworkReply*)),
			SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::doDownload(const QUrl &url) {
	QNetworkRequest request(url);
	QNetworkReply *reply = manager.get(request);

#if QT_CONFIG(ssl)
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
			SLOT(sslErrors(QList<QSslError>)));
#endif

	currentDownloads.append(reply);
}

QString DownloadManager::saveFileName(const QUrl &url) {
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();

	if (basename.isEmpty())
		basename = "download";

	QString targetname = DigitalRooster::RSS_FILE_DIR +QDir::separator()+ basename;

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

bool DownloadManager::saveToDisk(const QString &targetpath, QIODevice *data) {
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

bool DownloadManager::isHttpRedirect(QNetworkReply *reply) {
	int statusCode =
			reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	return statusCode == 301 || statusCode == 302 || statusCode == 303
			|| statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors) {
#if QT_CONFIG(ssl)
	for (const QSslError &error : sslErrors)
		fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
	Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply) {
	QUrl url = reply->url();
	if (reply->error()) {
		fprintf(stderr, "Download of %s failed: %s\n",
				url.toEncoded().constData(), qPrintable(reply->errorString()));
	} else {
		if (isHttpRedirect(reply)) {
			fputs("Request was redirected.\n", stderr);
		} else {
			QString filename = saveFileName(url);
			if (saveToDisk(filename, reply)) {
				emit newFileAvailable(filename);
			}
		}
	}

	currentDownloads.removeAll(reply);
	reply->deleteLater();
}

} /* namespace DigitalRooster */
