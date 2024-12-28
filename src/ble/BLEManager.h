#pragma once

#include "state/AppManager.h"

#define LR_FACTOR_MAX 99


class BLEManager {
public:
    AppManager *manager;
    BLEManager(AppManager* manager);

    void init(const char *device_name);
};