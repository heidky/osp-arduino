#pragma once

#include "state/AppManager.h"



class BLEManager {
public:
    AppManager *manager;
    BLEManager(AppManager* manager);

    void init(const char *device_name);

    static float vibe_level_to_mult(int level);
};