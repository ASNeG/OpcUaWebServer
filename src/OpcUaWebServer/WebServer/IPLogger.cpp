/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)

 */

#include <boost/filesystem.hpp>
#include "OpcUaWebServer/WebServer/IPLogger.h"
#include <iostream>

namespace OpcUaWebServer
{

	uint32_t IPLogger::counter_ = 0;
	std::string IPLogger::logFileName_ = "access.log";
	uint32_t IPLogger::maxLogFileNumber_ = 5;
	uint32_t IPLogger::maxLogFileSize_ = 100000;
	uint32_t IPLogger::maxIPAge_ = 3600000;
	boost::mutex IPLogger::mutex_;

	IPLogger::IPLogger(void)
	: isOpen_(false)
	, ofStream_()
	, ipMap_()
	{
	}

	IPLogger::~IPLogger(void)
	{
		closeLogFile();
	}

	void
	IPLogger::logFileName(const std::string logFileName)
	{
		logFileName_ = logFileName;
	}

	std::string
	IPLogger::logFileName(void)
	{
		return logFileName_;
	}

	void
	IPLogger::maxLogFileNumber(const uint32_t maxLogFileNumber)
	{
		maxLogFileNumber_ = maxLogFileNumber;
	}

	uint32_t
	IPLogger::maxLogFileNumber(void)
	{
		return maxLogFileNumber_;
	}

	void
	IPLogger::maxLogFileSize(const uint32_t maxLogFileSize)
	{
		maxLogFileSize_ = maxLogFileSize;
	}

	uint32_t
	IPLogger::maxLogFileSize(void)
	{
		return maxLogFileSize_;
	}

	void
	IPLogger::maxIPAge(const uint32_t maxIPAge)
	{
		maxIPAge_ = maxIPAge;
	}

	uint32_t
	IPLogger::maxIPAge(void)
	{
		return maxIPAge_;
	}

	bool
	IPLogger::openLogFile(void)
	{
		ofStream_.open(logFileName(), std::ios::out | std::ofstream::app);
		if (ofStream_) isOpen_ = true;
		return isOpen_ == true;
	}

	bool
	IPLogger::closeLogFile(void)
	{
		if (isOpen_) ofStream_.close();
		isOpen_ = false;
		return true;
	}

	void
	IPLogger::checkLogFile(void)
	{
		boost::filesystem::path logFile(logFileName());
		uint32_t logFileSize = boost::filesystem::file_size(logFile);
		if (logFileSize < maxLogFileSize_) return;

		// close log file
		closeLogFile();

		// rotate old log files
		rotateLogFile();

		openLogFile();
	}

	void
	IPLogger::rotateLogFile(void)
	{
		boost::filesystem::path logFile(logFileName());

		// remove oldest log file if necessary
		std::stringstream oldestLogFileName;
		oldestLogFileName << logFileName() << maxLogFileNumber_;
		boost::filesystem::remove(boost::filesystem::path(oldestLogFileName.str()));

		for (uint32_t idx = maxLogFileNumber_- 1; idx > 0; idx--) {
			std::stringstream srcLogFileName;
			std::stringstream dstLogFileName;

			srcLogFileName << logFileName() << idx;
			dstLogFileName << logFileName() << idx+1;

			if (boost::filesystem::exists(boost::filesystem::path(srcLogFileName.str()))) {
				boost::filesystem::rename(
					boost::filesystem::path(srcLogFileName.str()),
					boost::filesystem::path(dstLogFileName.str())
				);
			}
		}

		std::stringstream srcLogFileName;
		std::stringstream dstLogFileName;

		srcLogFileName << logFileName();
		dstLogFileName << logFileName() << "1";

		if (boost::filesystem::exists(boost::filesystem::path(srcLogFileName.str()))) {
			boost::filesystem::rename(
				boost::filesystem::path(srcLogFileName.str()),
				boost::filesystem::path(dstLogFileName.str())
			);
		}
	}

	void
	IPLogger::logout(const std::string& ip)
	{
		// find element in ip map
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		IPMap::iterator it;
		it = ipMap_.find(ip);
		if (it != ipMap_.end()) {
			boost::posix_time::time_duration td;
			td = now - it->second;
			if (td.total_milliseconds() < maxIPAge_) return;
			ipMap_.erase(it);
		}

		ipMap_.insert(std::make_pair(ip, now));


		// output ip
		if (!isOpen_) openLogFile();

		counter_++;
		if (counter_ > 500) {
			counter_ = 0;
			checkLogFile();
		}

		if (isOpen_) {
			ofStream_ << now << " " << ip << std::endl;
		}
		else {
			std::cout << now << " " << ip << std::endl;
		}
	}

}
