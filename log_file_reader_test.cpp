#include <iostream>
#include <gmock/gmock.h>

#include "log_file_reader.h"
#include "log_common.h"

using namespace datadog_log_analytics;

// Example for gtest
TEST(SimpleTest, LogReaderTest) {
    LogRecord log_record;
    const std::string kIP = "10.0.0.1";
    log_record.ip = kIP;
    EXPECT_EQ(log_record.ip, kIP);
}

// test to confirm line parsing
TEST(LogLineParseTest, LogReaderTest) {
    const std::string log_str = "\"10.0.0.2\",\" - \",\"apache\",1549573860,\"GET /api/user HTTP/1.0\",200,1234";

    LogReader log_reader;
    LogRecord log_record;
    EXPECT_EQ(log_reader.ParseLine(log_str, log_record), Status::OK);
    EXPECT_EQ(log_record.ip, "10.0.0.2");
    EXPECT_EQ(log_record.rfc, " - ");
    EXPECT_EQ(log_record.user, "apache");
    EXPECT_EQ(log_record.timestamp, 1549573860);
    EXPECT_EQ(log_record.full_request, "GET /api/user HTTP/1.0");
    EXPECT_EQ(log_record.section, "/api");
    EXPECT_EQ(log_record.status_code, 200);
    EXPECT_EQ(log_record.bytes, 1234);
}