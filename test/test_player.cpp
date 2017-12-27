/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/


#include <config.h>
#include <gtest/gtest.h>

#include <QMediaPlayer>
#include <QSignalSpy>
#include <QtDebug>

using namespace std;
using namespace DigitalRooster;

TEST(TestPlayer, no_media){
	QMediaPlayer mp;

	QSignalSpy spy(&mp,  SIGNAL(foo));
	auto url = QUrl("http://alternativlos.cdn.as250.net/alternativlos-40.mp3");
	mp.setMedia(url);
	mp.setPosition(900);
	mp.play();
	spy.wait(300);
	qDebug() << mp.position();
	mp.pause();
	mp.setPosition(2500);
	qDebug() << mp.position();
	mp.play();
	spy.wait(1500);
	auto media = mp.media().canonicalUrl();
	ASSERT_STREQ("http://alternativlos.cdn.as250.net/alternativlos-40.mp3",media.toString().toStdString().c_str());
	mp.play();
	spy.wait(500);
	qDebug() << mp.position();
	spy.wait(500);
}
