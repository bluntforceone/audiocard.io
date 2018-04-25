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
#include "linux/alsa/aio_alsa_ctl.h"
#include "linux/alsa/aio_alsa_pcm.h"
#include <iosfwd>
#include <sstream>

#include <bits/unique_ptr.h>
#include <functional>
#include <iostream>

namespace acio {

namespace {
    const unsigned int MAX_SAMPLE_RATES = 14;
    const unsigned int SAMPLE_RATES[] = {
        4000, 5512, 8000, 9600, 11025, 16000, 22050,
        32000, 44100, 48000, 88200, 96000, 176400, 192000
    };
}

Alsa::Alsa()
{
    int cardCount = this->countCards();
    this->_deviceCount = 0;
    for (int cardIndex = 0; cardIndex < cardCount; ++cardIndex) {
        int cardDeviceCount = this->countCardDevices(cardIndex);
        for (int cardDeviceIndex = 0; cardDeviceIndex < cardDeviceCount; ++cardDeviceIndex) {
            this->_deviceInfo.emplace_back(this->getCardDeviceInfo(cardIndex, cardDeviceIndex));
            ++this->_deviceCount;
        }
    }
    if (this->hasDefault()) {
        this->_deviceInfo.emplace_back(this->getCardDeviceInfo(-1, -1));
        ++this->_deviceCount;
    }
}

std::string alsaDeviceName(int cardIndex, int deviceIndex = -1)
{
    std::stringstream name;
    if (cardIndex > -1) {
        name << "hw:" << cardIndex;
        if (deviceIndex > -1) {
            name << "," << deviceIndex;
        }
    } else {
        name << "default";
    }
    return name.str();
}

std::string cardName(int cardIndex, int subdevice)
{
    if (cardIndex == -1) {
        return "default";
    }

    std::stringstream sCardName;
    char* cardname{ nullptr };
    int result = snd_card_get_name(cardIndex, &cardname);
    if (result >= 0) {
        sCardName << "hw:" << cardname << "," << subdevice;
        free(cardname);
    }
    return sCardName.str();
}

bool Alsa::hasDefault()
{
    SndCtl handle("default", 0);
    return (handle != nullptr);
}

int Alsa::countDevices()
{
    return this->_deviceCount;
}

DeviceInfo* Alsa::getDeviceInfo(int index)
{
    return &this->_deviceInfo[index];
}

int Alsa::countCards()
{
    int cardCount{ 0 };
    int cardIndex{ -1 };

    while ((snd_card_next(&cardIndex) == 0) && (cardIndex != -1)) {

        SndCtl handle(alsaDeviceName(cardIndex).c_str(), 0);

        if (handle) {
            ++cardCount;
        }
    }

    return cardCount;
}
int Alsa::countCardDevices(int cardIndex)
{
    int deviceCount{ 0 };
    int subDevice{ -1 };

    std::stringstream name;

    SndCtl handle(alsaDeviceName(cardIndex).c_str(), 0);

    if (!handle) {
        return 0;
    }

    while ((snd_ctl_pcm_next_device(&handle, &subDevice) == 0) && (subDevice != -1)) {
        ++deviceCount;
    }

    return deviceCount;
}

int Alsa::getAlsaDeviceIndex(int cardIndex, int deviceIndex)
{
    SndCtl handle(alsaDeviceName(cardIndex).c_str(), 0);

    int subDevice = -1;
    for (int currentIndex = 0; currentIndex < (deviceIndex + 1); ++currentIndex) {
        if (snd_ctl_pcm_next_device(&handle, &subDevice) != 0) {
            return -1;
        }
    }

    return subDevice;
}

int getDeviceChannelCount(snd_pcm_stream_t stream, snd_ctl_t* cHandle, int cardIndex, int alsaDeviceIndex)
{
    snd_pcm_info_t* pcmInfo{ nullptr };
    snd_pcm_info_alloca(&pcmInfo);
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    int result = 0;

    snd_pcm_info_set_stream(pcmInfo, stream);

    SndPcm pHandle(alsaDeviceName(cardIndex, alsaDeviceIndex).c_str(), stream, SND_PCM_ASYNC | SND_PCM_NONBLOCK);
    if (!pHandle) {
        return 0;
    }

    result = snd_pcm_hw_params_any(&pHandle, params);
    if (result < 0) {
        return 0;
    }

    unsigned int value{ 0 };
    result = snd_pcm_hw_params_get_channels_max(params, &value);

    return (result == 0 ? value : 0);
}

auto getDeviceSampleRates(snd_pcm_stream_t stream, int cardIndex, int deviceIndex)
{
    auto sampleRates = std::vector<unsigned int>();

    auto deviceName = alsaDeviceName(cardIndex, deviceIndex);

    SndPcm pHandle(deviceName.c_str(), stream, SND_PCM_ASYNC | SND_PCM_NONBLOCK);

    if (!pHandle) {
        return sampleRates;
    }

    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);

    int result = snd_pcm_hw_params_any(&pHandle, params);
    if (result < 0) {
        return sampleRates;
    }

    for (unsigned int i = 0; i < MAX_SAMPLE_RATES; i++) {
        if (snd_pcm_hw_params_test_rate(&pHandle, params, SAMPLE_RATES[i], 0) == 0) {
            sampleRates.push_back(SAMPLE_RATES[i]);
        }
    }

    return sampleRates;
}

DeviceInfo Alsa::getCardDeviceInfo(int cardIndex, int deviceIndex)
{
    DeviceInfo deviceInfo{};

    int alsaDeviceIndex = this->getAlsaDeviceIndex(cardIndex, deviceIndex);

    SndCtl cHandle(alsaDeviceName(cardIndex).c_str(), SND_CTL_NONBLOCK);

    if (!cHandle) {
        return deviceInfo;
    }

    auto maxChannelsStream = deviceInfo.inputChannels > deviceInfo.outputChannels ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK;

    deviceInfo.outputChannels = getDeviceChannelCount(SND_PCM_STREAM_PLAYBACK, &cHandle, cardIndex, alsaDeviceIndex);
    deviceInfo.inputChannels = getDeviceChannelCount(SND_PCM_STREAM_CAPTURE, &cHandle, cardIndex, alsaDeviceIndex);
    deviceInfo.sampleRates = getDeviceSampleRates(maxChannelsStream, cardIndex, alsaDeviceIndex);
    deviceInfo.name = cardName(cardIndex, alsaDeviceIndex);

    return deviceInfo;
}
}