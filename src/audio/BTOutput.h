#pragma once

#include "BluetoothA2DPOutput.h"


class BTOutput : public BluetoothA2DPOutputLegacy {
public:
    BTOutput() : BluetoothA2DPOutputLegacy() {};

    // virtual bool begin() override;
    // virtual void end() override;
    virtual size_t write(const uint8_t *data, size_t len) override;
    virtual void set_sample_rate(int rate) override;
    virtual void set_output_active(bool active) override;
};
