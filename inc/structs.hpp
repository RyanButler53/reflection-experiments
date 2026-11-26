#pragma once
#include <string>

namespace structs{
struct SubPacket {
    std::string substr;
    float data_float;
};

class DataPacket {
    int data;
    std::string name;
    SubPacket subpkt;
};

class DataPacket2 {
    int data;
    std::string name;
    SubPacket subpkt;
};
}
