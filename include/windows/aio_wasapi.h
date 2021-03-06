/****************************************************************************************************
 * Copyright 2018 Conrad Jones                                                                      *
 *                                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software    *
 * and associated documentation files (the "Software"), to deal in the Software without             *
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,       *
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the    *
 * Software is furnished to do so, subject to the following conditions:                             *
 *                                                                                                  *
 * The above copyright notice and this permission notice shall be included in all copies or         *
 * substantial portions of the Software.                                                            *
 *                                                                                                  *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING    *
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND       *
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,     *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.          *
 *                                                                                                  *
 ***************************************************************************************************/

#ifndef AUDIOCARD_IO_WASAPI_H
#define AUDIOCARD_IO_WASAPI_H

#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <Windows.h>
#include <vector>

#include "aio_audio.h"
#include "windows/com/aio_coinit.h"
#include "windows/com/aio_com.h"

namespace acio {

struct WasapiDeviceInfo : public DeviceInfo {
    bool supportsExclusive{ false };
    bool input{ false };
};

class Wasapi : public Audio {
public:
    Wasapi();

    ~Wasapi() = default;

public:
    int countDevices() override;
    DeviceInfo* getDeviceInfo(int64_t index) override;

    std::vector<int64_t> deviceIds() override;

    void enumDevices();

private:
    acom::CoInit coInit{ COINIT_APARTMENTTHREADED };
    std::vector<WasapiDeviceInfo> _devices;

    void enumDeviceEnumerator(IMMDeviceCollection* enumerator, bool input);

    void queryDevice(IMMDevice* pDevice, WasapiDeviceInfo& deviceInfo, bool input);
    void querySampleRates(IAudioClient* pAudioClient, WasapiDeviceInfo& deviceInfo);
    bool supportsExclusive(IAudioClient* pAudioClient);
};
}
#endif //AUDIOCARD_IO_WASAPI_H
