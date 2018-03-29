/*************************************************************************************
 * \filename PodcastSourceReader.cpp
 * \brief Implementation of the Class PodcastSourceReader
 *
 * \details
 *
 * \author  ruschi
 *************************************************************************************/

#include <appconstants.hpp>
#include "PodcastSourceReader.hpp"
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QVector>
#include <QString>
#include <QXmlStreamReader>
#include <memory>
#include <stdexcept> // std::system_error
using namespace DigitalRooster;

/*************************************************************************************/
static QTime tryParse(const QString& timestring){
	QVector<QString> formats{"hh:mm:ss","h:mm:ss","mm:ss","m:ss"};
	for(const auto &f : formats){
		auto time = QTime::fromString(timestring, f);
		if(time.isValid()){
			return time;
		}
	}
	// Last resort - we only have non compliant seconds... BBC does that..
	bool checkConversion = false;
	int secs = timestring.toInt(&checkConversion,10);

	if(checkConversion){
		return QTime(0,0).addSecs(secs);
	}
	return QTime(0,0);
}

/*************************************************************************************/
void parse_episodes(PodcastSource& podcastsource, QXmlStreamReader& xml) {
    // qDebug() << __FUNCTION__;

    if (xml.tokenType() != QXmlStreamReader::StartElement || xml.name() != "item") {
        qDebug() << "parse_episodes called but wrong XML element!";
        qDebug() << xml.name() << " Tokentype " << xml.tokenType();
        return;
    }

    auto ep = std::make_shared<PodcastEpisode>();

    while (!(xml.readNext() == QXmlStreamReader::EndElement && xml.name() == "item")) {

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.namespaceUri() == "") {
                if (xml.name() == "title") {
                    xml.readNext();
                    ep->set_display_name(xml.text().toString());
                } else if (xml.name() == "description") {
                    xml.readNext();
                    ep->set_description(xml.text().toString());
                } else if (xml.name() == "enclosure") {
                    // no readnext if we look at attributes
                    ep->set_url(QUrl(xml.attributes().value("url").toString()));
                } else if (xml.name() == "pubDate") {
                    xml.readNext();
                    ep->set_publication_date(QDateTime::fromString(
                        xml.text().toString(), Qt::DateFormat::RFC2822Date));
                }
            } else if (xml.namespaceUri() ==
                "http://www.itunes.com/dtds/podcast-1.0.dtd") {
                if (xml.name() == "duration") {
                    xml.readNext();
                    auto time = tryParse(xml.text().toString());
                    ep->set_duration(QTime(0,0).secsTo(time)*1000);
                }
            }
        }

        if (xml.hasError()) {
            throw std::invalid_argument(xml.errorString().toStdString());
        }
    }
    /* We want at least a display name and a media_url */
    if (ep->get_display_name().isEmpty()) {
        qWarning() << "Found channel with empty title: " << xml.lineNumber();
        return;
    }
    if (ep->get_url().isEmpty()) {
        qWarning() << "Found channel without media URL :" << xml.lineNumber();
        return;
    }
    podcastsource.add_episode(ep);
}

/*************************************************************************************/
void parse_channel(PodcastSource& podcastsource, QXmlStreamReader& xml) {
    // qDebug() << __FUNCTION__;

    if (!xml.isStartElement() || xml.name() != "channel") {
        qWarning() << "parse_channel called but wrong XML element!";
        return;
    }

    while (
        !(xml.readNext() == QXmlStreamReader::EndElement && xml.name() == "channel")) {

        /* only interpret standard RSS elements, not from itunes or atom namespace */
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.namespaceUri() == "") {
                // qDebug() << "StartElement ("<<xml.tokenType()<<")" << xml.name();
                if (xml.name() == "item") {
                    parse_episodes(podcastsource, xml);
                } else if (xml.name() == "title") {
                    xml.readNext();
                    // qDebug() << "title: "<< xml.name()<< " : " <<xml.text();
                    podcastsource.set_title(xml.text().toString());
                } else if (xml.name() == "description") {
                    xml.readNext();
                    // qDebug() << "description: "<< xml.name()<< " : " <<xml.text();
                    podcastsource.set_description(xml.text().toString());
                } else if (xml.name() == "link") {
                    xml.readNext();
                    podcastsource.set_link(xml.text().toString());
                }
            } else if (xml.namespaceUri() ==
                "http://www.itunes.com/dtds/podcast-1.0.dtd") {
                if (xml.name() == "image") {
                    podcastsource.set_image_uri(
                        QUrl(xml.attributes().value("href").toString()));
                }
            }
        }
        if (xml.hasError()) {
            throw std::invalid_argument(xml.errorString().toStdString());
        }

        // qDebug() << __FUNCTION__ <<  xml.name() << " TokenType: " <<
        // xml.tokenType();
    }
}

/*************************************************************************************/
void DigitalRooster::update_podcast(PodcastSource& podcastsource) {
    //	qDebug() << __FUNCTION__ << "("<< podcastsource.get_rss_file() << ")";

    QFile file(podcastsource.get_rss_file());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        throw std::system_error(
            make_error_code(std::errc::no_such_file_or_directory), "Cannot read file");
    }

    QXmlStreamReader xml(&file);
    xml.setNamespaceProcessing(true);

    // loop the entire file, a rss is really flat
    try {
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
                }
            }
        }
    } catch (std::invalid_argument& exc) {
        qWarning() << " XML error in line:" << xml.lineNumber() << exc.what();
        return;
    }
    //    qDebug() <<"parsing o.k.";
}
