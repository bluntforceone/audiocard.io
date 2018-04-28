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

#include "linux/aio_alsa.h"
#include "linux/alsa/aio_alsa_cardinfo.h"
#include "linux/alsa/aio_alsa_ctl.h"
#include "linux/alsa/aio_alsa_deviceinfo.h"
#include "linux/alsa/aio_alsa_pcm.h"
#include <iosfwd>
#include <map>
#include <sstream>

#include <memory>
#include <functional>
#include <iostream>

namespace acio {

Alsa::Alsa()
{
    this->enumDevices();
}

bool Alsa::hasDefault()
{
    SndCtl handle("default", 0);
    return (handle != nullptr);
}

int Alsa::countDevices()
{
    return this->deviceIds().size();
}

DeviceInfo* Alsa::getDeviceInfo(int64_t deviceId)
{
    AlsaDeviceIdMap internalDeviceId(deviceId);

    auto cardIterator = this->_cardMap.find(internalDeviceId.alsaCardId);
    if (cardIterator == this->_cardMap.end()) {
        return nullptr;
    }

    auto deviceIterator = (*cardIterator).second.find(internalDeviceId.alsaDeviceId);
    if (deviceIterator == (*cardIterator).second.end()) {
        return nullptr;
    }

    return &(*deviceIterator).second;
}

Alsa::AlsaDeviceInfo Alsa::getCardDeviceInfo(int cardIndex, int deviceIndex)
{
    AlsaDeviceInfo deviceInfo{};

    SndCtl cHandle(alsaDeviceId(cardIndex), SND_CTL_NONBLOCK);

    if (!cHandle) {
        return deviceInfo;
    }

    const auto deviceId = alsaDeviceId(cardIndex, deviceIndex);

    deviceInfo.outputChannels = aslaDeviceChannelCount(SND_PCM_STREAM_PLAYBACK, deviceId);
    deviceInfo.inputChannels = aslaDeviceChannelCount(SND_PCM_STREAM_CAPTURE, deviceId);
    auto maxChannelsStream = deviceInfo.inputChannels > deviceInfo.outputChannels ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK;
    deviceInfo.sampleRates = aslaDeviceSampleRates(maxChannelsStream, deviceId);
    deviceInfo.name = aslaDisplayName(cardIndex, deviceIndex);

    return deviceInfo;
}

void Alsa::enumDevices()
{
    auto cardIds = alsaCardIds();

    for (auto cardId : cardIds) {
        auto deviceMap = std::map<int, AlsaDeviceInfo>();
        auto deviceIds = alsaPcmDeviceIds(cardId);

        for (auto deviceId : deviceIds) {
            deviceMap.emplace(deviceId, this->getCardDeviceInfo(cardId, deviceId));
        }
        if (deviceMap.size() > 0) {
            this->_cardMap.emplace(cardId, std::move(deviceMap));
        }
    }

    if (this->hasDefault()) {
        auto deviceMap = std::map<int, AlsaDeviceInfo>();
        deviceMap.emplace(-1, this->getCardDeviceInfo(-1, -1));
        this->_cardMap.emplace(-1, std::move(deviceMap));
    }
}

std::vector<int64_t> Alsa::deviceIds()
{
    std::vector<int64_t> ids;
    for (auto& card : this->_cardMap) {
        for (auto& device : card.second) {
            ids.emplace_back(AlsaDeviceIdMap(card.first, device.first).deviceId);
        }
    }
    return ids;
}
}