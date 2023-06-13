//
// Created by leeiozh on 6/13/23.
//

#include "SeaVision/Input/FileReader.hpp"
#include "gtest/gtest.h"

namespace SeaVision {

TEST(TEST_FILE_READER, READER) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.04/");

    FileReader reader(path);
    reader.read_one_file("20221004-115006-351.bt8");


}
}