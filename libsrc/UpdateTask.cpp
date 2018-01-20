/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#include "PodcastSourceReader.hpp"
#include "UpdateTask.hpp"
#include "DownloadManager.hpp"
#include "PodcastSource.hpp"

namespace DigitalRooster {

UpdateTask::UpdateTask(std::shared_ptr<PodcastSource> source)
    : ps(source)
    , dlm(std::make_unique<DownloadManager>()) {
    connect(dlm.get(), SIGNAL(newFileAvailable(const QString&)), this,
        SLOT(newFileAvailable(const QString&)));
}

UpdateTask::~UpdateTask() {
}

void UpdateTask::newFileAvailable(const QString& filename) {
    ps->set_rss_file(filename);
    update_podcast(*ps);
    emit completed();
}

void UpdateTask::start() {
    dlm->doDownload(ps->get_url());
}

} /* namespace DigitalRooster */
