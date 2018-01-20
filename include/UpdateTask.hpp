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


namespace DigitalRooster {

class PodcastSource;
class DownloadManager;

class UpdateTask: public QObject {
Q_OBJECT
public:
	explicit UpdateTask(std::shared_ptr<PodcastSource> source);
	virtual ~UpdateTask();

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
	void completed();

private:
	/**
	 * local copy of podcast source to update
	 */
	std::shared_ptr<PodcastSource> ps;

	/**
	 * handles downloads
	 */
	std::unique_ptr<DownloadManager> dlm;
};

} /* namespace DigitalRooster */

#endif /* INCLUDE_UPDATETASK_HPP_ */
