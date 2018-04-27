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

#include "aio_asio.h"
#include <array>
#include <asio.h>
#include <memory>

#include <iostream>

namespace acio {

namespace {

    const int MAX_SAMPLE_RATES = 14;
    const std::array<unsigned int, MAX_SAMPLE_RATES> SAMPLE_RATES{
        4000, 5512, 8000, 9600, 11025, 16000, 22050,
        32000, 44100, 48000, 88200, 96000, 176400, 192000
    };
}

struct AsioDeviceIdMap {
    union {
        int64_t deviceId {0};
        struct {
            int asioIndex;
        };
    };
    AsioDeviceIdMap() = default;
    AsioDeviceIdMap(int64_t deviceId_)
            : deviceId(deviceId_)
    {
    }
    static AsioDeviceIdMap fromInt(int asioIndex)
    {
        AsioDeviceIdMap idMap;
        idMap.asioIndex = asioIndex;
        return idMap;
    }
};

Asio::Asio()
{
    this->enumDevices();
}

void Asio::enumDevices()
{
    auto driverCount = this->asioDrivers.asioGetNumDev();

    for (long index = 0; index < driverCount; ++index) {
        auto driverName = std::string();
        driverName.resize(32);
        this->asioDrivers.asioGetDriverName(index, driverName.data(), 32);
        this->asioDrivers.loadDriver(driverName.c_str());

        auto driverInfo = ASIODriverInfo();
        ASIOInit(&driverInfo);

        auto deviceInfo = DeviceInfo();

        deviceInfo.name = driverName;
        long inputChannels{ 0 };
        long outputChannels{ 0 };

        ASIOGetChannels(&inputChannels, &outputChannels);
        deviceInfo.inputChannels = inputChannels;
        deviceInfo.outputChannels = outputChannels;
        deviceInfo.changeableInputChannelCount = false;
        deviceInfo.changeableOutputChannelCount = false;

        ASIOChannelInfo channelInfo{};

        if (inputChannels > 0) {
            channelInfo.isInput = true;
            channelInfo.channel = 0;
            ASIOGetChannelInfo(&channelInfo);
        }
        if (outputChannels > 0) {
            channelInfo.isInput = true;
            channelInfo.channel = 0;
            ASIOGetChannelInfo(&channelInfo);
        }

        if (channelInfo.type == ASIOSTInt16MSB || channelInfo.type == ASIOSTInt16LSB) {
            deviceInfo.nativeFormats |= AudioFormat::SINT16;
        } else if (channelInfo.type == ASIOSTInt32MSB || channelInfo.type == ASIOSTInt32LSB) {
            deviceInfo.nativeFormats |= AudioFormat::SINT32;
        } else if (channelInfo.type == ASIOSTFloat32MSB || channelInfo.type == ASIOSTFloat32LSB) {
            deviceInfo.nativeFormats |= AudioFormat::FLOAT32;
        } else if (channelInfo.type == ASIOSTFloat64MSB || channelInfo.type == ASIOSTFloat64LSB) {
            deviceInfo.nativeFormats |= AudioFormat::FLOAT64;
        } else if (channelInfo.type == ASIOSTInt24MSB || channelInfo.type == ASIOSTInt24LSB) {
            deviceInfo.nativeFormats |= AudioFormat::SINT24;
        }

        for (size_t index = 0; index < MAX_SAMPLE_RATES; ++index) {
            auto result = ASIOCanSampleRate((ASIOSampleRate)SAMPLE_RATES[index]);
            if (result == ASE_OK) {
                deviceInfo.sampleRates.push_back(SAMPLE_RATES[index]);
            }
        }

        this->asioDrivers.removeCurrentDriver();
        this->devices.emplace_back(deviceInfo);
    }
}

int Asio::countDevices()
{
    return static_cast<int>(this->devices.size());
}

DeviceInfo* Asio::getDeviceInfo(int64_t deviceId)
{
    auto idMap = AsioDeviceIdMap(deviceId);
    if (idMap.asioIndex >= this->devices.size()) {
        return nullptr;
    }
    return &this->devices[idMap.asioIndex];
}

std::vector<int64_t> Asio::deviceIds()
{
    std::vector<int64_t> ids;
    for (int index = 0; index < this->devices.size(); ++index)
    {
        auto idMap = AsioDeviceIdMap::fromInt(index);
        ids.emplace_back(idMap.deviceId);
    }
    return ids;
}
}
