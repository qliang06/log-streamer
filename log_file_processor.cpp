#include <istream>

#include "log_file_processor.h"

namespace datadog_log_analytics {
LogFileProcessor::LogFileProcessor() {

}

LogFileProcessor::~LogFileProcessor() {
    
}

void LogFileProcessor::SetTrafficThreshold(float threshold) {
    two_mins_rule_.SetTrafficThreshold(threshold);
}

Status::ReturnStatus LogFileProcessor::ProcessRecord(const LogRecord& log_record) {
    Status::ReturnStatus return_status = Status::OK;
    // check with 10s rule
    return_status = ten_secs_rule_.ProcessRecord(log_record);
    if (return_status != Status::OK) {
        std::cerr << "Failed to analyze past 10s realtime section hits on record: " << log_record << std::endl;
        return return_status;
    }

    // check 2 mins rule
    return_status = two_mins_rule_.ProcessRecord(log_record);
    if (return_status != Status::OK) {
        std::cout << "Failed to analyze past 10mins traffic:" << log_record << std::endl;
    }

    return return_status;
}

// LogRealTimeAnalysis (10s rule)
LogRealTimeAnalysis::LogRealTimeAnalysis() {
    // initlaize starting time to 0
    prev_timestamp_ = 0;
    stat_.section_cnt = 0;
}

LogRealTimeAnalysis::~LogRealTimeAnalysis() {
    //print last piece before exit
    PrintStats_();
}

void LogRealTimeAnalysis::Reset() {
    prev_timestamp_  = 0;
    stat_.section_cnt = 0;
    stat_.traffic_bytes = 0;
    stat_.section.clear();
    section_meta_map_.clear();
}

void LogRealTimeAnalysis::PrintStats_() {
    std::cout << "Section: " << stat_.section << ", Hits: " << stat_.section_cnt 
              << " at time: " << stat_.timestamp << std::endl;    
}

void LogRealTimeAnalysis::DebugStats_() {
    std::cout << "prev_timestamp_:" << prev_timestamp_;
    for (const auto& item :section_meta_map_) {
        std::cout << item.first << " -> " << item.second << std::endl;
    }
    std::cout << "Stat:" << stat_ << std::endl;

}

const AnalysisMeta& LogRealTimeAnalysis::GetCurrentStat() const {
    return stat_;
}

Status::ReturnStatus LogRealTimeAnalysis::ProcessRecord(const LogRecord& record) {
    // Validate log record
    if (record.section.empty() || record.bytes <= 0) {
        std::cerr << "Invalid log:" << record << std::endl;
        return Status::INVALID_LOG_RECORD;
    }

    // first record in 10s
    if (prev_timestamp_ == 0) {
        AnalysisMeta meta_data;
        meta_data.timestamp = record.timestamp;
        meta_data.section = record.section;
        meta_data.section_cnt = 1;
        meta_data.traffic_bytes = record.bytes;

        section_meta_map_[record.section] = meta_data;
        stat_ = meta_data;
        prev_timestamp_ = record.timestamp;
        
        return Status::OK;
    }

    // compare with prev and update every 10 seconds
    if (record.timestamp - prev_timestamp_ >= 10) {
        PrintStats_();
        Reset();
        prev_timestamp_ = record.timestamp;
    }

    // regular flow
    if (section_meta_map_.find(record.section) == section_meta_map_.end()) {
        AnalysisMeta meta_data;
        meta_data.timestamp = record.timestamp;
        meta_data.section_cnt = 1;
        meta_data.traffic_bytes = record.bytes;
        meta_data.section = record.section;
        section_meta_map_[record.section] = meta_data;
    } else {
        section_meta_map_[record.section].section_cnt ++;
        section_meta_map_[record.section].traffic_bytes += record.bytes;
    }

    // update stat_ to highest hits
    stat_ = section_meta_map_[record.section].section_cnt > stat_.section_cnt ? section_meta_map_[record.section] : stat_;
    
    return Status::OK;
}

LogMonitor::LogMonitor() {
    current_timestamp_ = 0;
    traffic_sum_ = 0.0;
    traffic_threshold_ = 10.0; // default threshold to 10.0
    traffic_status_normal_ = true;
}

LogMonitor::LogMonitor(float traffic_threshold) {
    current_timestamp_ = 0;
    traffic_sum_ = 0.0;
    traffic_threshold_ = traffic_threshold;
    traffic_status_normal_ = true;
}

LogMonitor::~LogMonitor() {

}

void LogMonitor::SetTrafficThreshold(float traffic_threshold) {
    traffic_threshold_ = traffic_threshold;
}

const long long& LogMonitor::GetCurrentTime() const {
    return current_timestamp_;
}

const float& LogMonitor::GetTrafficSum() const {
    return traffic_sum_;
}

void LogMonitor::Debug() const {
    std::cout 
          << "heap size = " << log_heap_.size() 
          << ", current_timestamp = " << current_timestamp_
          << ", start_time = " << log_heap_.top().timestamp
          << "\nalert_stats = " << traffic_status_normal_
          << ", threashold = " << traffic_threshold_
          << ", traffic_sum = " << traffic_sum_
          << std::endl;
}

Status::ReturnStatus LogMonitor::ProcessRecord(const LogRecord& record) {
    // Validate log record
    if (record.section.empty() || record.bytes <= 0) {
        std::cerr << "Invalid log:" << record << std::endl;
        return Status::INVALID_LOG_RECORD;
    }
    
    if (current_timestamp_ == 0) {
        log_heap_.push(record);
        traffic_sum_ += 1.0;
        current_timestamp_ = record.timestamp;
    } else {
        // ignore obsolete record
        if (record.timestamp < current_timestamp_ - 120) {
            return Status::OK;
        }

        log_heap_.push(record);
        traffic_sum_ += 1.0;
        current_timestamp_ = current_timestamp_ > record.timestamp ? current_timestamp_ : record.timestamp;

        while (!log_heap_.empty() && log_heap_.top().timestamp < current_timestamp_ - 120) {
            log_heap_.pop();
            traffic_sum_ -= 1.0;
        }
    }

    // check average hits per second
    long long time_span = current_timestamp_ - log_heap_.top().timestamp;
    float avg_hits = traffic_sum_;
    
    if (time_span > 0) {
        avg_hits = traffic_sum_ / time_span;
    } 
    
    // total hits above threshold traffic and not alerted before
    if (traffic_status_normal_ && avg_hits >= traffic_threshold_) {
        std::cout << "High traffic generated an alert - hits = " << traffic_sum_ << ", at time:" << current_timestamp_ << ", threadhold = " << traffic_threshold_ << " hits/sec" << std::endl;
        traffic_status_normal_ = false;
    }

    // total hits recovers and status was not normal
    if (!traffic_status_normal_ && avg_hits < traffic_threshold_) {
        std::cout << "Traffic recovered to normal at time: " << current_timestamp_ << std::endl;
        traffic_status_normal_ = true;
    }
    return Status::OK;
}

} // namespace datadog_log_analytics