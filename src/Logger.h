/*!
* \file Logger.h
* \brief Write log in console and file at the same time.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once
#pragma warning(disable : 4996)

#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>

#ifndef logE
#define logE Logger::log() << " ERROR @ "
#define logW Logger::log() << " WARNING @ "
#define logI Logger::log() << " INFO @ "
#endif

class Logger
{
public:
	Logger(std::ostream& os) : m_os(os), m_fs("log.txt") {}
	~Logger() {
		if (m_fs.is_open())
			m_fs.close();
	}

	template<typename T>
	Logger& operator<<(const T& data)
	{
		m_fs << data;
		m_os << data;

		return *this;
	}
	Logger& operator<<(std::ostream&(*f)(std::ostream&))
	{
		m_fs << f;
		m_os << f;

		return *this;
	}

	/*!
	* \brief Obtains log singleton object.
	* \return logger.
	*/
	static Logger& log() {

		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);
		auto now_tm = std::localtime(&now_time_t);

		m_logger.m_fs << "[" << std::put_time(now_tm, "%c") << "] ";
		m_logger.m_os << "[" << std::put_time(now_tm, "%c") << "] ";

		return m_logger;
	}

private:
	static Logger m_logger; /* Logger singleton object */
	std::ostream& m_os;		/* output stream for logs */
	std::ofstream m_fs;		/* output file */
};


