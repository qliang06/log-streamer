#include <iostream>
#include <gmock/gmock.h>

#include "log_common.h"
#include "log_file_processor.h"
#include "log_file_reader.h"

int main(int argc, char** argv) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}