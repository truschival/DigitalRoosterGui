/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#include <UpdateTask.hpp>
#include <PodcastSourceReader.hpp>

namespace DigitalRooster {

UpdateTask::UpdateTask(std::shared_ptr<PodcastSource> source): ps(source) {
	connect(&dlm,SIGNAL(newFileAvailable(const QString&)),this,SLOT(newFileAvailable(const QString&)));

}

UpdateTask::~UpdateTask() {
	// TODO Auto-generated destructor stub
}

void UpdateTask::newFileAvailable(const QString& filename) {
//	qDebug() << __FUNCTION__ << "("<<filename<<")";
	ps->set_rss_file(filename);
	update_podcast(*ps);
	emit completed();
}

void UpdateTask::start() {
	dlm.doDownload(ps->get_url());
}

} /* namespace DigitalRooster */
