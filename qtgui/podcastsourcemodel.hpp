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
class MediaPlayerProxy;
} // namespace DigitalRooster

class PodcastEpisodeModel;

class PodcastSourceModel : public QAbstractListModel {
    Q_OBJECT
public:
    PodcastSourceModel(DigitalRooster::IPodcastStore&  store,
        std::shared_ptr<DigitalRooster::MediaPlayerProxy> pp,
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

    Q_INVOKABLE void refresh(int index);
    Q_INVOKABLE void purge(int index);
    Q_INVOKABLE void remove(int index);

public slots:
    void newDataAvailable();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    DigitalRooster::IPodcastStore&  cm;
    std::shared_ptr<DigitalRooster::MediaPlayerProxy> mpp;
};

#endif /* QTGUI_PODCASTSOURCEMODEL_HPP_ */
