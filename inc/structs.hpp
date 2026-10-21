#pragma once
#include <string>

struct SubPacket {
    std::string substr;
    float data_float;
};

class DataPacket {
    int data;
    std::string name;
    SubPacket subpkt;
};
