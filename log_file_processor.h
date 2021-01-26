#ifndef LOG_FILE_PROCESSOR_H
#define LOG_FILE_PROCESSOR_H

#include <unordered_map>
#include <queue>
#include <vector>

#include "log_common.h"

namespace datadog_log_analytics {

// check log every 10 sec to find section with highest hits
class LogRealTimeAnalysis {
public:
    LogRealTimeAnalysis ();
    virtual ~LogRealTimeAnalysis();
    LogRealTimeAnalysis(const LogRealTimeAnalysis& rhs) = delete;
    LogRealTimeAnalysis operator = (const LogRealTimeAnalysis& rhs) = delete;

    void Reset();
    Status::ReturnStatus ProcessRecord(const LogRecord& log_record);
    const AnalysisMeta& GetCurrentStat() const;

private:
    // Start time stamp for 10s
    time_t prev_timestamp_;
    // aggregate meta data by section into a map
    std::unordered_map<std::string, AnalysisMeta> section_meta_map_;
    AnalysisMeta stat_;

    void PrintStats_();
    void DebugStats_();
};

// comparator for min heap
static bool RecordCmp(LogRecord a, LogRecord b) { return a.timestamp > b.timestamp;}

// monitoring log for the past 2mins, 
// utilize heap to maintain 2mins window.
class LogMonitor {
public:
    LogMonitor();
    explicit LogMonitor(float traffic_threshold);
    virtual ~LogMonitor();
    LogMonitor(const LogMonitor& rhs) = delete;
    LogMonitor operator = (const LogMonitor) = delete;

    void SetTrafficThreshold(float traffic_threshold);
    Status::ReturnStatus ProcessRecord(const LogRecord& log_record);
    const long long& GetCurrentTime() const;
    const float& GetTrafficSum() const;
    void Debug() const;

private:
    std::priority_queue<LogRecord, std::vector<LogRecord>, decltype(&RecordCmp)> log_heap_{RecordCmp};
    long long current_timestamp_;
    float traffic_threshold_;
    float traffic_sum_; // traffic = hits
    bool  traffic_status_normal_;
};

// class to process log record, including 2 member class to process
// 10s rule and 2mins rule independently
class LogFileProcessor {
public:
    LogFileProcessor();
    virtual ~LogFileProcessor();
    LogFileProcessor(const LogFileProcessor& rhs) = delete;
    LogFileProcessor operator = (const LogFileProcessor& rhs) = delete;

    Status::ReturnStatus ProcessRecord(const LogRecord& log_record); 
    void SetTrafficThreshold(float threshold);

private:
    LogRealTimeAnalysis ten_secs_rule_;
    LogMonitor          two_mins_rule_;
    
};

} // namespace datadog_log_analytics

#endif