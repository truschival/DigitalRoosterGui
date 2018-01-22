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

#include "PodcastSource.hpp"
#include <QAbstractListModel>
#include <QObject>
#include <QStringList>

namespace DigitalRooster {

class ConfigurationManager;

class PodcastSourceModel : public QAbstractListModel {
    Q_OBJECT
public:
    PodcastSourceModel(ConfigurationManager* confman, QObject* parent = nullptr);

    enum PodcastSourceRoles {
        DisplayNameRole = Qt::UserRole + 1,
        DisplayUrlRole,
        DisplayCountRole,
		DescriptionRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

public slots:
    void newDataAvailable();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    ConfigurationManager* cm;
};
}
#endif /* QTGUI_PODCASTSOURCEMODEL_HPP_ */
