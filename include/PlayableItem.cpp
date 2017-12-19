/*************************************************************************************
 * \filename PlayableItem.cpp
 * \brief Implementation of the Class PlayableItem
 *
 * \details
 *
 * \author  ruschi 
 *************************************************************************************/


#include "PlayableItem.hpp"


PlayableItem::PlayableItem(){

}



PlayableItem::~PlayableItem(){

}





/*************************************************************************************/
QString& PlayableItem::get_display_name(){

	return display_name;
}


/*************************************************************************************/
QUrl PlayableItem::get_media_url(){

	return media_url;
}


PodCastEpisode::PodCastEpisode(){

}



PodCastEpisode::~PodCastEpisode(){

}





/*************************************************************************************/
QString PodCastEpisode::get_guid(){

	return guid;
}


SeekablePlayableItem::SeekablePlayableItem(){

}



SeekablePlayableItem::~SeekablePlayableItem(){

}





/*************************************************************************************/
bool SeekablePlayableItem::completed(){

	return false;
}


/*************************************************************************************/
int SeekablePlayableItem::get_position_percent(){

	return position_percent;
}


/*************************************************************************************/
void SeekablePlayableItem::set_position_percent(int newVal){

	position_percent = newVal;
}


PodcastEpisode::PodcastEpisode(){

}



PodcastEpisode::~PodcastEpisode(){

}





/*************************************************************************************/
QString& PodcastEpisode::get_author(){

	return author;
}


/*************************************************************************************/
QString& PodcastEpisode::get_description(){

	return description;
}


/*************************************************************************************/
QString PodcastEpisode::get_guid(){

	return guid;
}