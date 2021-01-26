#include <iostream>
#include <fstream>

#include "log_common.h"
#include "log_file_reader.h"
#include "log_file_processor.h"

using namespace datadog_log_analytics;

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "Arg should be:\n" << argv[0] << "<log file> <traffic threshold(optional)" << std::endl;
        return 0;
    }
    
    std::ifstream in_file(argv[1]);
    if (!in_file) {
        std::cerr << "Uable to open file" << std::endl;
	   return Status::OPEN_FILE_FAILURE;
    }

    LogReader log_reader;
    LogRecord log_record;
    LogFileProcessor log_file_processor;
    if (argc >= 3) {
        log_file_processor.SetTrafficThreshold(std::atof(argv[2]));
    }

    // while readline in file. Ignore the header
    std::string log_str;
    std::getline(in_file, log_str);

    while (std::getline(in_file, log_str)) {
        // parse line into LogRecord with API log_file_reader
        if (log_reader.ParseLine(log_str, log_record) != Status::OK) {
            std::cerr << "Unable to parse the line" << std::endl;
            continue;
        }
       
        // process LogRecord with API in log_file_processor
        Status::ReturnStatus return_status = log_file_processor.ProcessRecord(log_record);
        if (return_status != Status::OK) {
           std::cerr << "Unable to process the log record" << std::endl;
           continue;
        }
    }
    
    in_file.close(); 
    
    return 0;
}
