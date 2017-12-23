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

	if (xml.tokenType() != QXmlStreamReader::StartElement
			|| xml.name() != "item") {
		qDebug() << "parse_episodes called but wrong XML element!";
		qDebug() << xml.name() << " Tokentype " << xml.tokenType();
		return;
	}

	QString ressource;
	QString title;

	while (!(xml.readNext() == QXmlStreamReader::EndElement
			&& xml.name() == "item")) {

		if (xml.tokenType() == QXmlStreamReader::StartElement) {
			if (xml.name() == "title") {
				xml.readNext();
				title = xml.text().toString();
			} else if (xml.name() == "enclosure") {
				// no readnext if we look at attributes
				ressource = xml.attributes().value("url").toString();
			}
		}
	}
	if (!ressource.isEmpty() && !title.isEmpty()) {
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

	while (!(xml.readNext() == QXmlStreamReader::EndElement
			&& xml.name() == "channel")) {

		if (xml.tokenType() == QXmlStreamReader::StartElement) {
			//qDebug() << "StartElement ("<<xml.tokenType()<<")" << xml.name();
			if (xml.name() == "item") {
				parse_episodes(podcastsource, xml);
			} else if (xml.name() == "title") {
				xml.readNext();
				podcastsource.set_title(xml.text().toString());
			} else if (xml.name() == "description") {
				xml.readNext();
				podcastsource.set_description(xml.text().toString());
			} else if (xml.name() == "link") {
				podcastsource.set_link(xml.text().toString());
			} else {
				//qDebug() << xml.name();
			}
		}
		//qDebug() << __FUNCTION__ <<  xml.name() << " TokenType: " << xml.tokenType();

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

		if (token == QXmlStreamReader::StartDocument) {
			continue;
		}
		if (token == QXmlStreamReader::StartElement) {

			if (xml.name() == "rss") {
				qDebug() << xml.name() << " // continue";
				continue;
			} else if (xml.name() == "channel") {
				qDebug() << xml.name() << " // parse_channel";
				parse_channel(podcastsource, xml);
//			} else if (xml.name() == "item") {
//				qDebug() << "  from " << __FUNCTION__;
//				parse_episodes(podcastsource, xml);
			} else {
				//qDebug() << xml.name();
			}
		}
	}
}
