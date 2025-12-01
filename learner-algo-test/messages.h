#pragma once

#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

class UpdateLearnerMessage {
public:    
    vector<uint32_t> bytes;
    uint32_t message_length;
    uint32_t origin_rep_id;
};