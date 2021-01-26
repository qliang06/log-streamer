#include <iostream>

#include "log_common.h"

namespace datadog_log_analytics {

std::ostream& operator<< (std::ostream& os, const LogRecord& record) {
    os << "IP:" << record.ip << "|"
       << "rfc:" << record.rfc << "|"
       << "user:" << record.user << "|"
       << "timestamp:" << record.timestamp << "|"
       << "request:" << record.full_request << "|"
       << "status:" << record.status_code << "|"
       << "bytes:" << record.bytes;
   return os;
}

std::ostream& operator << (std::ostream& os, const AnalysisMeta& data) {
    os << "Section:" << data.section << "|"
       << "Section Count:" << data.section_cnt << "|"
       << "Traffic Bytes:" << data.traffic_bytes << "|"
       << "Error Count:" << data.error_cnt;
    return os;
}
} // namespace datadog_log_analytics