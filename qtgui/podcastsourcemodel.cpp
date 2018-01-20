/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#include <QByteArray>
#include <QHash>

#include "configuration_manager.hpp"
#include "podcastsourcemodel.hpp"

using namespace DigitalRooster;

PodcastSourceModel::PodcastSourceModel(ConfigurationManager* confman, QObject* parent)
    : QAbstractListModel(parent)
    , cm(confman) {
}

QHash<int, QByteArray> PodcastSourceModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[DisplayNameRole] = "display_name";
    roles[DisplayUrlRole] = "link";
    return roles;
}

int PodcastSourceModel::rowCount(const QModelIndex& /*parent*/) const {
    return cm->get_podcast_sources().size();
}

QVariant PodcastSourceModel::data(const QModelIndex& index, int role) const {
    auto v = cm->get_podcast_sources();
    if (index.row() < 0 || index.row() >= v.size())
        return QVariant();

    auto ps = v[index.row()];

    if (role == DisplayNameRole) {
        return QVariant(ps->get_title());
    } else if (role == DisplayUrlRole) {
        return ps->get_link();
    }
    return QVariant();
}
