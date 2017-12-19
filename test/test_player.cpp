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
	auto url = QUrl("file:///usr/share/sounds/Oxygen-Im-Connection-Lost.ogg");
	mp.setMedia(url);
	mp.play();
	spy.wait(300);
	qDebug() << mp.position();
	mp.pause();
	qDebug() << mp.position();
	auto media = mp.media().canonicalUrl();
	ASSERT_STREQ("file:///usr/share/sounds/Oxygen-Im-Connection-Lost.ogg",media.toString().toStdString().c_str());
	mp.play();
	spy.wait(500);
	qDebug() << mp.position();
	spy.wait(500);
}
