#include "JsonParser.h"

#include <regex>

JsonParser& JsonParser::add(const std::string& key, const std::string& value)
{
	m_data[key] = value;
	return *this;
}

std::string JsonParser::get(const std::string& key)
{
	if (m_data.find(key) != m_data.end())
		return m_data[key];
	else
		return std::string();
}

std::string JsonParser::getJson()
{
	std::string json = "{";
	for (auto pair : m_data) {
		json += "\"" + pair.first + "\" : \"" + pair.second + "\",";
	}
	json.replace(json.end() - 1, json.end(), "}");
	return json;
}

int JsonParser::decode(char* json)
{
	m_data.clear();
	int err = 1;
	std::string data(json);
	std::regex  regex("\"(.*?)\"|:([ 0-9-]+),|:([ 0-9-]+)\\}");
	auto words_begin = std::sregex_iterator(data.begin(), data.end(), regex);
	auto words_end = std::sregex_iterator();

	try {
		for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
			std::smatch match = *i++;
			std::string key = match.str();
			key.erase(0, 1);
			key.erase(key.length() - 1);

			match = *i;
			std::string value = match.str();
			value.erase(0, 1);
			value.erase(value.length() - 1);

			m_data[key] = value;
		}
	}
	catch (std::exception) {
		err = -1;
	}
	return err;
}