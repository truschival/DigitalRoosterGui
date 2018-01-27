/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#ifndef INCLUDE_UPDATETASK_HPP_
#define INCLUDE_UPDATETASK_HPP_
#include <QObject>
#include <QString>
#include <memory>
#include "DownloadManager.hpp"

namespace DigitalRooster {

class PodcastSource;

class UpdateTask: public QObject {
Q_OBJECT
public:
	explicit UpdateTask(PodcastSource& source);
	virtual ~UpdateTask() = default;

public slots:
	/**
	 * Called by DownloadManager when download completed
	 * @param filename
	 */
	void newFileAvailable(const QString& filename);

	/**
	 * Starts download and parsing
	 */
	void start();

signals:
	/**
	 * Download and parsing has completed
	 */
	void newDataAvailable();

private:
	/**
	 * local copy of podcast source to update
	 */
	PodcastSource& ps;

	/**
	 * handles downloads
	 */
	DownloadManager dlm;
	/**
	 * Filehash of last download
	 */
	QByteArray lasthash;
};

} /* namespace DigitalRooster */

#endif /* INCLUDE_UPDATETASK_HPP_ */
