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

#ifndef INCLUDE_UPDATETASK_HPP_
#define INCLUDE_UPDATETASK_HPP_

#include <memory>
#include <chrono>

#include <QObject>
#include <QString>
#include <QTimer>
#include "DownloadManager.hpp"


namespace DigitalRooster {

class PodcastSource;

class UpdateTask : public QObject {
    Q_OBJECT
public:
    explicit UpdateTask(PodcastSource* source = nullptr);
    virtual ~UpdateTask() = default;

    /**
     * Set the PodcastSourceAutoupdating
     * will restart the timer if interval is less than current update_interval
     * @param interval
     */
    void set_update_interval(std::chrono::seconds interval);

    /**
     * Source to update when timer elapsed
     * @param ps
     */
    void set_podcast_source(PodcastSource* ps);

public slots:
    /**
     * Called by DownloadManager when download completed
     * @param data content of networkreply
     */
    void dataAvailable(const QByteArray& data);
    /**
     * Starts download and parsing
     */
    void start();

signals:
    /**
     * Download and parsing has completed
     */
    void dataChanged();

private:
    /**
     * podcast source to update
     */
    PodcastSource* ps;

    /**
     * handles downloads
     */
    DownloadManager dlm;

    /**
     * Periodic timer for update action
     */
    QTimer timer;
};

} /* namespace DigitalRooster */

#endif /* INCLUDE_UPDATETASK_HPP_ */
