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

#include "osx/aio_coreaudio.h"
#include <iostream>

namespace acio {

struct CoreAudioIdMap {
    union {
        int64_t deviceId;
        int32_t coreAudioCardId;
    };
    CoreAudioIdMap(int64_t deviceId_)
        : deviceId(deviceId_)
    {
    }
    CoreAudioIdMap(AudioObjectID coreAudioCardId_)
        : coreAudioCardId(coreAudioCardId_)
    {
    }
};

CoreAudio::CoreAudio()
{
    UInt32 io_size{};
    OSStatus os_err{};

    AudioObjectPropertyAddress prop_address = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    os_err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &prop_address, 0, NULL, &io_size);
    if (os_err != 0) {
        std::cout << "Error:" << os_err << std::endl;
        return;
    }

    auto deviceIds = std::vector<AudioObjectID>();
    deviceIds.resize(io_size / sizeof(AudioObjectID));
    os_err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &prop_address, 0, NULL, &io_size, deviceIds.data());
    if (os_err != 0) {
        std::cout << "Error:" << os_err << std::endl;
        return;
    }

    for (auto deviceId : deviceIds) {

        auto deviceInfo = DeviceInfo();

        cf::String deviceName;

        prop_address.mSelector = kAudioObjectPropertyName;
        prop_address.mScope = kAudioObjectPropertyScopeGlobal;
        prop_address.mElement = kAudioObjectPropertyElementMaster;
        io_size = sizeof(CFStringRef);

        os_err = AudioObjectGetPropertyData(deviceId, &prop_address, 0, NULL, &io_size, &deviceName.stringRef);
        if (os_err != 0) {
            std::cout << "Error:" << os_err << std::endl;
            continue;
        }
        deviceInfo.name = deviceName.stdString();
        this->_deviceInfo.emplace(deviceId, std::move(deviceInfo));
    }
}

int CoreAudio::countDevices()
{
    return this->_deviceInfo.size();
}

DeviceInfo* CoreAudio::getDeviceInfo(int64_t deviceId)
{
    auto coreAudioId = CoreAudioIdMap(deviceId);

    auto deviceIterator = this->_deviceInfo.find(coreAudioId.coreAudioCardId);
    if (deviceIterator == this->_deviceInfo.end()) {
        return nullptr;
    }

    return &(*deviceIterator).second;
}

std::vector<int64_t> CoreAudio::deviceIds()
{
    auto ids = std::vector<int64_t>();
    for (auto& device : this->_deviceInfo) {
        auto coreAudioId = CoreAudioIdMap(device.first);
        ids.emplace_back(coreAudioId.deviceId);
    }
    return ids;
}
}