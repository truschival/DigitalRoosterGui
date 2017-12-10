/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/
#ifndef MEDIAPLAYER_WRAPPER_HPP_
#define MEDIAPLAYER_WRAPPER_HPP_

#include <QObject>
#include <memory>
#include <QMediaPlayer>
#include <QDebug>

#include <iostream>

class MediaPlayerWrapper : public QObject {
	Q_OBJECT
public:
	MediaPlayerWrapper() : pimpl(new QMediaPlayer){
		qDebug() << __FUNCTION__;
		std::cout << "iostreams:  " << __FUNCTION__ << std::endl;
	};


	virtual ~MediaPlayerWrapper(){

	};


	void getMedia(){
		qDebug() << __FUNCTION__;
		auto media =  pimpl->media().canonicalUrl();
		qDebug() << media;
	}


public slots:

	void play(){
		qDebug() << __FUNCTION__;
		pimpl->play();
	}

	void play(const QUrl& url){
		qDebug() << __FUNCTION__;
		pimpl->setMedia(QMediaContent(url));
		pimpl->play();
	}

	void pause(){
		qDebug() << __FUNCTION__;
		qDebug() << " foo";
		pimpl->pause();
	}

private:
	std::unique_ptr<QMediaPlayer> pimpl;
};

#endif /* MEDIAPLAYER_WRAPPER_HPP_ */
