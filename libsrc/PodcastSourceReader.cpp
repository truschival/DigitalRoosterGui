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
#include <QDir>
#include <QFile>

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
void PodcastSourceReader::parse_episodes(const QString& rss_feed) {

}

/*************************************************************************************/
void PodcastSourceReader::parse_info(const QString& rss_feed) {

}

/*************************************************************************************/
void PodcastSourceReader::update_episodes(PodcastSourceConfiguration& config) {

}

/*************************************************************************************/
void PodcastSourceReader::update_info(PodcastSourceConfiguration& config) {

}
