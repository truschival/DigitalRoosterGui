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
	//qDebug() << __FUNCTION__;

	if (xml.tokenType() != QXmlStreamReader::StartElement
			|| xml.name() != "item") {
		qDebug() << "parse_episodes called but wrong XML element!";
		qDebug() << xml.name() << " Tokentype " << xml.tokenType();
		return;
	}

	QString ressource;
	QString title;
	QString desc;
	QDateTime dt;

	while (!(xml.readNext() == QXmlStreamReader::EndElement
			&& xml.name() == "item")) {

		if (xml.tokenType() == QXmlStreamReader::StartElement
				&& xml.namespaceUri() == "") {
			if (xml.name() == "title") {
				xml.readNext();
				title = xml.text().toString();
			} else if (xml.name() == "description") {
				xml.readNext();
				desc = xml.text().toString();
			}
			else if (xml.name() == "enclosure") {
				// no readnext if we look at attributes
				ressource = xml.attributes().value("url").toString();
			} else if (xml.name() == "pubDate") {
				xml.readNext();
				dt=	QDateTime::fromString(xml.text().toString(),
								Qt::DateFormat::RFC2822Date);
			}

		}

		if(xml.hasError()){
			throw std::invalid_argument(xml.errorString().toStdString());
		}
	}
	if (!ressource.isEmpty() && !title.isEmpty()) {
		auto ep = std::make_shared<PodcastEpisode>(title, QUrl(ressource));

		if(!desc.isEmpty())
			ep->description=desc;
		if(dt.isValid())
			ep->publication_date = dt;

		podcastsource.add_episode(ep);
	} else {
		qDebug() << "incomplete item! " << ressource << " : " << title;
	}

}

/*************************************************************************************/
void PodcastSourceReader::parse_channel(
		PodcastSourceConfiguration& podcastsource, QXmlStreamReader& xml) {
	//qDebug() << __FUNCTION__;

	if (!xml.isStartElement() || xml.name() != "channel") {
		qDebug() << "parse_channel called but wrong XML element!";
		return;
	}

	while (!(xml.readNext() == QXmlStreamReader::EndElement
			&& xml.name() == "channel")) {

		/* only interpret standard RSS elements, not from itunes or atom namespace */
		if (xml.tokenType() == QXmlStreamReader::StartElement
				&& xml.namespaceUri() == "") {
			//qDebug() << "StartElement ("<<xml.tokenType()<<")" << xml.name();
			if (xml.name() == "item") {
				parse_episodes(podcastsource, xml);
			} else if (xml.name() == "title") {
				xml.readNext();
				//qDebug() << "title: "<< xml.name()<< " : " <<xml.text();
				podcastsource.set_title(xml.text().toString());
			} else if (xml.name() == "description") {
				xml.readNext();
				//qDebug() << "description: "<< xml.name()<< " : " <<xml.text();
				podcastsource.set_description(xml.text().toString());
			} else if (xml.name() == "link") {
				xml.readNext();
				podcastsource.set_link(xml.text().toString());
			} else if (xml.name() == "pubDate") {
				xml.readNext();
				qDebug() << "last updated:"
						<< QDateTime::fromString(xml.text().toString(),
								Qt::DateFormat::RFC2822Date);
				podcastsource.set_last_updated(
						QDateTime::fromString(xml.text().toString(),
								Qt::DateFormat::RFC2822Date));
			}
		}

		if(xml.hasError()){
			throw std::invalid_argument(xml.errorString().toStdString());
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
	//qDebug() << __FUNCTION__;

	QFile file(podcastsource.get_rss_feed());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << file.errorString();
// maybe throw() ?
		return;
	}

	QXmlStreamReader xml(&file);
	xml.setNamespaceProcessing(true);

// loop the entire file, a rss is really flat
	try{
	while (!xml.atEnd() && !xml.hasError()) {
		QXmlStreamReader::TokenType token = xml.readNext();

		if (token == QXmlStreamReader::StartDocument) {
			continue;
		}
		if (token == QXmlStreamReader::StartElement) {
			if (xml.name() == "rss") {
				continue;
			} else if (xml.name() == "channel") {
				parse_channel(podcastsource, xml);
			} else {
				//qDebug() << xml.name();
			}
		}
	}}
	catch (std::invalid_argument& exc){
		qDebug() << " XML error in line:" << xml.lineNumber() << exc.what();
	}
}
