/*************************************************************************************
 * \filename
 * \brief	QML abstract model
 *
 * \details needed to bind a list of episodes to QML
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 *
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *************************************************************************************/
#ifndef QTGUI_PODCASTEPISODEMODEL_HPP_
#define QTGUI_PODCASTEPISODEMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
#include <memory>


namespace DigitalRooster {

class ConfigurationManager;
class PodcastEpisode;

}
class PodcastEpisodeModel : public QAbstractListModel {
    Q_OBJECT
public:

    PodcastEpisodeModel(QObject* parent = nullptr);

    PodcastEpisodeModel(const QVector<std::shared_ptr<DigitalRooster::PodcastEpisode>>* episodes,
        QObject* parent = nullptr);

    enum PodcastEpisodeRoles {
        DisplayNameRole = Qt::UserRole + 1,
        DurationRole,
        CurrentPositionRole,
        DescriptionRole,
        DateRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    void set_episodes(const QVector<std::shared_ptr<DigitalRooster::PodcastEpisode>>* episodes);

    const QString& getName() {
        return name;
    }

    void setName(const QString& n) {
        name = n;
    }

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    const QVector<std::shared_ptr<DigitalRooster::PodcastEpisode>>* episodes;

    QString name;
};


#endif
