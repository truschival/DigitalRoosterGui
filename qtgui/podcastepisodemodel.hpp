/*****************************************************************************
 * \filename
 * \brief	QML abstract model
 *
 * \details needed to bind a list of episodes to QML
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 *
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 ******************************************************************************/
#ifndef QTGUI_PODCASTEPISODEMODEL_HPP_
#define QTGUI_PODCASTEPISODEMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
#include <memory>


namespace DigitalRooster {

class ConfigurationManager;
class PodcastEpisode;
class MediaPlayer;

/**
 * Model for list of Episodes of one podcast source
 */
class PodcastEpisodeModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ get_current_index WRITE set_current_index
            NOTIFY current_index_changed)
public:
    PodcastEpisodeModel(
        const QVector<std::shared_ptr<PodcastEpisode>>* episodes,
        MediaPlayer& mp, QObject* parent = nullptr);

    enum PodcastEpisodeRoles {
        DisplayNameRole = Qt::UserRole + 1,
        TitleRole,
        PublisherRole,
        DurationRole,
        CurrentPositionRole,
        DescriptionRole,
        DateRole,
        ListenedRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    void set_episodes(const QVector<std::shared_ptr<PodcastEpisode>>* episodes);

    const QString& getName() {
        return name;
    }

    void setName(const QString& n) {
        name = n;
    }

    void set_current_index(int val) {
        currentIndex = val;
        emit current_index_changed(currentIndex);
    }

    int get_current_index() {
        return currentIndex;
    }

    Q_INVOKABLE DigitalRooster::PodcastEpisode* get_episode(int index);
    Q_INVOKABLE void send_to_player(int index);

signals:
    void current_index_changed(int newIndex);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    const QVector<std::shared_ptr<PodcastEpisode>>* episodes;
    MediaPlayer& mpp;
    int currentIndex = -1;
    QString name;
};

} // namespace DigitalRooster

#endif
