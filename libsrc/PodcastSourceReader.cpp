/*************************************************************************************
 * \filename PodcastSourceReader.cpp
 * \brief Implementation of the Class PodcastSourceReader
 *
 * \details
 *
 * \author  ruschi 
 *************************************************************************************/

#include "config.h"
#include "PodcastSourceReader.hpp"
#include <QDebug>
#include <QFile>
#include <memory>

using namespace DigitalRooster;

PodcastSourceReader::PodcastSourceReader() {

}

PodcastSourceReader::~PodcastSourceReader() {

}

/*************************************************************************************/
PodcastSourceConfiguration PodcastSourceReader::create_PodcastSource(
		const QString& display_name, const QString& url, size_t max_episodes) {
	return PodcastSourceConfiguration("");
}

/*************************************************************************************/
QString PodcastSourceReader::download_rss(const QString& url) {
	return QString("alternativlos.rss");
}

/*************************************************************************************/
void PodcastSourceReader::parse_episodes(
		PodcastSourceConfiguration& podcastsource, QXmlStreamReader& xml) {
	qDebug() << __FUNCTION__;

	if (!xml.isStartElement() || xml.name() != "item") {
		qDebug() << "parse_episodes called but wrong XML element!";
		return;
	}

	QString ressource;
	QString title;

	while (xml.readNextStartElement()) {
		if (xml.name() == "title") {
			title = xml.readElementText();
		} else if (xml.name() == "enclosure") {
			ressource = xml.attributes().value("url").toString();
		} else {
			xml.skipCurrentElement();
		}
	}
	if (! ressource.isEmpty() &&  !title.isEmpty()) {
		auto ep = std::make_shared<PodcastEpisode>(title, QUrl(ressource));
		podcastsource.add_episode(ep);
	} else {
		qDebug() << "incomplete item! " << ressource << " : " << title;

	}

}

/*************************************************************************************/
void PodcastSourceReader::parse_channel(
		PodcastSourceConfiguration& podcastsource, QXmlStreamReader& xml) {
	qDebug() << __FUNCTION__;

	if (!xml.isStartElement() || xml.name() != "channel") {
		qDebug() << "parse_channel called but wrong XML element!";
		return;
	}

	while (xml.readNextStartElement()) {
		if (xml.name() == "item") {
			parse_episodes(podcastsource, xml);
		} else if (xml.name() == "title") {
			podcastsource.set_title(xml.readElementText());
		} else if (xml.name() == "description") {
			podcastsource.set_description(xml.readElementText());
		} else if (xml.name() == "link") {
			podcastsource.set_link(xml.readElementText());
		} else {
			xml.skipCurrentElement();
		}
	}
}

/*************************************************************************************/
void PodcastSourceReader::update_episodes(PodcastSourceConfiguration& config) {

}

/*************************************************************************************/
void PodcastSourceReader::update_podcast(
		PodcastSourceConfiguration& podcastsource) {
	qDebug() << __FUNCTION__;

	QFile file(podcastsource.get_rss_feed());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << file.errorString();
// maybe throw() ?
		return;
	}

	QXmlStreamReader xml(&file);
// loop the entire file, a rss is really flat
	while (!xml.atEnd() && !xml.hasError()) {
		QXmlStreamReader::TokenType token = xml.readNext();

		// Skip ahead to channel
		if (token == QXmlStreamReader::StartElement
				&& xml.name() == "channel") {
			parse_channel(podcastsource, xml);
		}
		if (xml.tokenType() == QXmlStreamReader::Invalid)
			xml.readNext();

		if (xml.hasError()) {
			xml.raiseError();
			qDebug() << xml.errorString();
		}
	}
}
