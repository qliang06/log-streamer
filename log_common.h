#ifndef LOG_COMMON_H
#define LOG_COMMON_H

#include <iostream>

namespace datadog_log_analytics {

struct Status {
    enum ReturnStatus {
        OK = 0,
        OPEN_FILE_FAILURE,
        INVALID_LOG_RECORD
    };
    ReturnStatus return_status;
    std::string  return_msg;
    Status() : return_status(OK) {}
};

struct LogRecord {
    std::string ip;
    std::string rfc;
    std::string user;
    long long timestamp; 
    std::string full_request;
    std::string section;
    int status_code;
    long long bytes;
    LogRecord() : timestamp(0), status_code(0), bytes(0) {}
    friend std::ostream& operator<< (std::ostream& os, const LogRecord& record);
};

// Meta data for analysis 10secons status analysis
struct AnalysisMeta {
    std::string section;
    long long timestamp;
    long long section_cnt;
    long long traffic_bytes;
    long long error_cnt;
    AnalysisMeta() : timestamp(0), section_cnt(0), traffic_bytes(0), error_cnt(0) {}
    friend std::ostream& operator << (std::ostream& os, const AnalysisMeta& data);
};

} // namespace datadog_log_analytics

#endif