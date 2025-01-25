#pragma once

#include "BluetoothA2DPOutput.h"
#include "audio/I2SRouter.h"


class BtOutput : public BluetoothA2DPOutput {
public:
    BtOutput() : BluetoothA2DPOutput() {};
    I2SRouter *router = nullptr;

    virtual bool begin() override;
    virtual void end() override;
    virtual size_t write(const uint8_t *data, size_t len) override;
    virtual void set_sample_rate(int rate) override;
    virtual void set_output_active(bool active) override;
protected:

    int32_t sample_rate = 44100;
    bool i2s_active = true;
};
