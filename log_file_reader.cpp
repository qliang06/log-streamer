#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

#include "log_file_reader.h"

namespace datadog_log_analytics {

LogReader::LogReader() {

}

LogReader::LogReader(const std::string& file_name) {
    file_name_ = std::move(file_name);
}

LogReader::~LogReader() {

}

const std::string& LogReader::GetFileName() {
    return file_name_;
}

Status::ReturnStatus LogReader::Initialize() {
    return Status::OK;
}

// API to parse line into LogLine
Status::ReturnStatus LogReader::ParseLine(const std::string& log_str, LogRecord& log_record) {
    // split file and parse into 
	std::string line(log_str);
	line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
	std::istringstream iss(line);
	std::string temp;
	std::vector<std::string> svec;

	while (std::getline(iss, temp, ',')) {
		svec.push_back(temp);
	}

	int index = 0;

	log_record.ip = svec[index++];
	log_record.rfc = svec[index++];
	log_record.user = svec[index++];
	log_record.timestamp = std::stoll(svec[index++]);

	temp = svec[index++];
	log_record.full_request = temp;
	std::size_t found1 = temp.find('/');
	std::size_t found2 = temp.find(' ', found1);
	std::size_t found3 = temp.find('/', found1+1);
	log_record.section = temp.substr(found1, std::min(found2, found3) - found1);

	log_record.status_code = std::stoi(svec[index++]);
	log_record.bytes = std::stoll(svec[index++]);

    return Status::OK;
} 

} // namespace datadog_log_analytics