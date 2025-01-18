#pragma once

#include <SoundData.h>
#include "audio/helpers.h"
#include "state/AppManager.h"

class CommonProcessing {
public:
    CommonProcessing();

    void process(State &state, Frame *data, size_t frame_count, int32_t SR);
    void reset();
private:
    uint32_t time = 0;

    Mult l_mult;
    Mult r_mult;
    Mult vibe_mult;
    Mult remote_mult;
};