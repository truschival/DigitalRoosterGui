/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace DigitalRooster{

	/**
	 * Application CompileTime Configurations
	 */
	const std::string SYSTEM_CONFIG_PATH("/tmp/dr.ini");


	/**
	 * Runtime configuration object
	 */
	class AppConfig{
	public:
		std::string mediaFile;
	};

}
