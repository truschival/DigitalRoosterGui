/*************************************************************************************
 * \filename StreamSource.cpp
 * \brief Implementation of the Class StreamSource
 *
 * \details
 *
 * \author  ruschi 
 *************************************************************************************/


#include "StreamSource.hpp"


RadioStreamSource::StreamSource(){

}



RadioStreamSource::~RadioStreamSource(){

}





/*************************************************************************************/
QString& RadioStreamSource::get_display_name(){

	return display_name;
}


/*************************************************************************************/
QString& RadioStreamSource::get_url(){

	return url_string;
}