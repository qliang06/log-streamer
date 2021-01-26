#ifndef LOG_FILE_READER_H
#define LOG_FILE_READER_H

#include <fstream>
#include <string>
#include <ctime>

#include "log_common.h"

namespace datadog_log_analytics {

class LogReader {
public:
    LogReader();
    explicit LogReader(const std::string& file_name);
    virtual ~LogReader();
    LogReader(const LogReader& rhs) = delete;
    

    const std::string& GetFileName();
    std::ifstream& GetFileStream();

    Status::ReturnStatus Initialize();

    // Parse line into LogRecord
    Status::ReturnStatus ParseLine(const std::string& log_str, LogRecord& log_record);     
private:
    std::string file_name_;
    std::ifstream file_stream_;
};

} // datadog_log_analytics

#endif