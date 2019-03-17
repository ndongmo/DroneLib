/*!
* \file JsonParser.h
* \brief Json parser class.
* \author Ndongmo Silatsa Fabrice
* \date 09-03-2019
* \version 1.0
*/

#pragma once

#include <map>
#include <string>

class JsonParser
{
public:
	JsonParser& add(const std::string& key, const std::string& value);
	int decode(char* json);
	std::string get(const std::string& key);
	std::string getJson();

private:
	std::map<std::string, std::string> m_data;
};

