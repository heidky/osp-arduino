#pragma once

#include <SoundData.h>
#include "audio/helpers.h"
#include "state/AppManager.h"

class BtProcessing {
public:
    BtProcessing();

    void process(State &state, Frame *data, size_t frame_count, int32_t SR);
    void reset();
private:
    uint32_t time = 0;

    ExpAvg intensity_l = ExpAvg(0.001);
    ExpAvg intensity_r = ExpAvg(0.001);
};
