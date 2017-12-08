/*************************************************************************************
 * \filename    
 * \brief
 *
 * \details
 * 
 * \author ruschi
 * 
 *
 *************************************************************************************/


#ifndef _STREAMSOURCE_HPP_
#define _STREAMSOURCE_HPP_ 

/**
 * Internet radio stream meta information
 */
class RadioStreamSource
{

public:
	RadioStreamSource();
	virtual ~RadioStreamSource();

	/*************************************************************************************/
	const QString& RadioStreamSource::get_display_name(){

		return display_name;
	}


	/*************************************************************************************/
	const QString& RadioStreamSource::get_url(){

		return url_string;
	}

private:
	QString display_name;
	QString url_string;

};
#endif // _STREAMSOURCE_HPP_ 
