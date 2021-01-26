#include <gmock/gmock.h>
#include <iostream>

#include "log_file_processor.h"
#include "log_common.h"

using namespace datadog_log_analytics;

class LogFileProcessorTest : public ::testing::Test {
public:
    void SetUp() {
    }
    void TearDown() {
    }
private:
    LogFileProcessor log_proc_;
};

class LogMonitorTest : public ::testing::Test {
public:
    void SetUp() {
        monitor_ptr_ = new LogMonitor(2);
    }
    void TearDown() {
        delete monitor_ptr_;
    }
    LogMonitor* monitor_ptr_ ;
};

TEST_F(LogMonitorTest, ProcessSmallBatch) {
    LogRecord record;
    record.timestamp = 1549574332;
    record.section = "/api";
    record.bytes = 1234;
    EXPECT_EQ(monitor_ptr_->ProcessRecord(record), Status::OK);
    record.timestamp = 1549574332;
    record.section = "/api";
    record.bytes = 1000;
    EXPECT_EQ(monitor_ptr_->ProcessRecord(record), Status::OK);
    record.timestamp = 1549574332;
    record.section = "/api";
    record.bytes = 1000;
    EXPECT_EQ(monitor_ptr_->ProcessRecord(record), Status::OK);
    EXPECT_EQ(monitor_ptr_->GetTrafficSum(), 3);
    // expect an alert here

    record.timestamp = 1549574335;
    record.section = "/api";
    record.bytes = 1111;
    EXPECT_EQ(monitor_ptr_->ProcessRecord(record), Status::OK);
    EXPECT_EQ(monitor_ptr_->GetTrafficSum(), 4);
    // still high traffic, but not alerting since already alerted

    record.timestamp = 1549574635;
    record.section = "/api";
    record.bytes = 1111;
    EXPECT_EQ(monitor_ptr_->ProcessRecord(record), Status::OK);
    EXPECT_EQ(monitor_ptr_->GetCurrentTime(), record.timestamp);
    EXPECT_EQ(monitor_ptr_->GetTrafficSum(), 1);
    // expect recovery alert

    record.timestamp = 1549574636;
    record.section = "/api";
    record.bytes = 1111;
    EXPECT_EQ(monitor_ptr_->ProcessRecord(record), Status::OK);
    EXPECT_EQ(monitor_ptr_->GetCurrentTime(), record.timestamp);
    EXPECT_EQ(monitor_ptr_->GetTrafficSum(), 2);
    // nothing, no alert
}

class LogRealTimeAnalysisTest : public ::testing::Test {
public:
    void SetUp() {}
    void TearDown() {}
    LogRealTimeAnalysis analyzer_;
};

TEST_F(LogRealTimeAnalysisTest, ProcessSingleRecord) {
    LogRecord record;
    record.timestamp = 1549574332;
    record.section = "/api";
    record.bytes = 1234;

    EXPECT_EQ(analyzer_.ProcessRecord(record), Status::OK);
    EXPECT_EQ(analyzer_.GetCurrentStat().section, record.section);
    EXPECT_EQ(analyzer_.GetCurrentStat().section_cnt, 1);
    EXPECT_EQ(analyzer_.GetCurrentStat().traffic_bytes, record.bytes);
}

TEST_F(LogRealTimeAnalysisTest, ProcessBatchRecords) {
    analyzer_.Reset();
    LogRecord api_record;
    api_record.section = "/api";
    api_record.bytes = 1000;

    LogRecord user_record;
    user_record.section = "/user";
    user_record.bytes = 1111;

    api_record.timestamp = 1549571000;
    EXPECT_EQ(analyzer_.ProcessRecord(api_record), Status::OK);
    user_record.timestamp = 1549571001;
    EXPECT_EQ(analyzer_.ProcessRecord(user_record), Status::OK);
    api_record.timestamp = 1549571001;
    EXPECT_EQ(analyzer_.ProcessRecord(api_record), Status::OK);
    user_record.timestamp = 1549571002;
    EXPECT_EQ(analyzer_.ProcessRecord(user_record), Status::OK);
    api_record.timestamp = 1549571005;
    EXPECT_EQ(analyzer_.ProcessRecord(api_record), Status::OK);
    api_record.timestamp = 1549571007;
    EXPECT_EQ(analyzer_.ProcessRecord(api_record), Status::OK);
    EXPECT_EQ(analyzer_.GetCurrentStat().section, api_record.section);
    EXPECT_EQ(analyzer_.GetCurrentStat().section_cnt, 4);
    EXPECT_EQ(analyzer_.GetCurrentStat().traffic_bytes, 4 * api_record.bytes);

    user_record.timestamp = 1549571012;
    EXPECT_EQ(analyzer_.ProcessRecord(user_record), Status::OK);
    EXPECT_EQ(analyzer_.GetCurrentStat().section, user_record.section);
    EXPECT_EQ(analyzer_.GetCurrentStat().section_cnt, 1);
    EXPECT_EQ(analyzer_.GetCurrentStat().traffic_bytes, 1 * user_record.bytes);

}