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

namespace DigitalRooster {
	class ConfigurationManager;
	class MediaPlayerProxy;
}

class PodcastEpisodeModel;

class PodcastSourceModel : public QAbstractListModel {
    Q_OBJECT
 public:
    PodcastSourceModel(DigitalRooster::ConfigurationManager* confman,
		DigitalRooster::MediaPlayerProxy* pp,
		QObject* parent = nullptr);

    enum PodcastSourceRoles {
        DisplayNameRole = Qt::UserRole + 1,
        DisplayUrlRole,
        DisplayCountRole,
        DescriptionRole,
        ImageRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE PodcastEpisodeModel* get_episodes(int index);

public slots:
    void newDataAvailable();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    DigitalRooster::ConfigurationManager* cm;
	DigitalRooster::MediaPlayerProxy* mpp;
};

#endif /* QTGUI_PODCASTSOURCEMODEL_HPP_ */
