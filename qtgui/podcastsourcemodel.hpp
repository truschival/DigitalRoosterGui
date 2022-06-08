/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#ifndef QTGUI_PODCASTSOURCEMODEL_HPP_
#define QTGUI_PODCASTSOURCEMODEL_HPP_


#include <QAbstractListModel>
#include <QObject>
#include <QStringList>
#include <memory>

#include "IPodcastStore.hpp"

namespace DigitalRooster {
class MediaPlayer;
class PodcastEpisodeModel;

/**
 * ListModel for Podcast Stations
 */
class PodcastSourceModel : public QAbstractListModel {
    Q_OBJECT
public:
	/**
	 * Create a PodcastSource model
	 * @param store info on PodcastSources
	 * @param mp Mediaplayer will be passed on to EpisodesModel
	 * @param parent
	 */
    PodcastSourceModel(
        IPodcastStore& store, MediaPlayer& mp, QObject* parent = nullptr);

    enum PodcastSourceRoles {
        DisplayNameRole = Qt::UserRole + 1,
        DisplayUrlRole,
        DisplayCountRole,
        DescriptionRole,
        ImageRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE DigitalRooster::PodcastEpisodeModel* get_episodes(int index);

    Q_INVOKABLE void refresh(int index);
    Q_INVOKABLE void purge(int index);
    Q_INVOKABLE void remove(int index);

public slots:
    void newDataAvailable();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    IPodcastStore& config;
    MediaPlayer& mpp;
};

} // namespace DigitalRooster
#endif /* QTGUI_PODCASTSOURCEMODEL_HPP_ */
