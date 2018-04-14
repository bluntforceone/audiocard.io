#include "linux/aio_alsa.h"
#include <alsa/asoundlib.h>
#include <iosfwd>
#include <sstream>

#include <iostream>

namespace acio {

int Alsa::countDevices()
{
    int deviceCount = this->countAllSubDevices();

    snd_ctl_t* handle{ nullptr };
    if (snd_ctl_open(&handle, "default", 0) == 0) {
        ++deviceCount;
        snd_ctl_close(handle);
    }

    return deviceCount;
}

DeviceInfo Alsa::getDeviceInfo(int index)
{

    int deviceCount = this->countDevices();

    if (index < 0 || index >= deviceCount) {
        return DeviceInfo {};
    }

    int cardCount = this->countCards();
    for (int cardIndex = 0; cardIndex < cardCount; ++cardIndex) {
        int cardDeviceCount = this->countCardDevices(cardIndex);
        if (index < cardDeviceCount) {
            return this->getCardDeviceInfo(cardIndex, index);
        }
        index -= cardDeviceCount;
    }

    return this->getCardDeviceInfo(-1, -1);
}

int Alsa::countCards()
{
    int cardCount{ 0 };
    int card{ -1 };
    snd_ctl_t* handle{ nullptr };

    while ((snd_card_next(&card) == 0) && (card != -1)) {
        std::stringstream name;
        name << "hw:" << card;

        if (snd_ctl_open(&handle, name.str().c_str(), 0) != 0) {
            continue;
        }

        ++cardCount;

        snd_ctl_close(handle);
    }

    return cardCount;
}
int Alsa::countCardDevices(int cardIndex)
{
    int deviceCount{ 0 };
    int subDevice{ -1 };
    snd_ctl_t* handle{ nullptr };
    std::stringstream name;
    name << "hw:" << cardIndex;

    if (snd_ctl_open(&handle, name.str().c_str(), 0) != 0) {
        return 0;
    }

    while ((snd_ctl_pcm_next_device(handle, &subDevice) == 0) && (subDevice != -1)) {
        ++deviceCount;
    }

    snd_ctl_close(handle);

    return deviceCount;
}

int Alsa::countAllSubDevices()
{
    int deviceCount{ 0 };
    int cardCount = this->countCards();
    for (int cardIndex = 0; cardIndex < cardCount; ++cardIndex) {
        deviceCount += this->countCardDevices(cardIndex);
    }
    return deviceCount;
}

int Alsa::getAlsaDeviceIndex(int cardIndex, int deviceIndex)
{
    snd_ctl_t* handle{ nullptr };
    std::stringstream name;
    name << "hw:" << cardIndex;

    if (snd_ctl_open(&handle, name.str().c_str(), 0) != 0) {
        return -1;
    }

    int subDevice = -1;
    for (int currentIndex = 0; currentIndex < (deviceIndex + 1); ++currentIndex) {
        if (snd_ctl_pcm_next_device(handle, &subDevice) != 0) {
            return -1;
        }
    }

    snd_ctl_close(handle);
    return subDevice;
}

int getDeviceOutputChannelCount(snd_pcm_stream_t stream, snd_ctl_t* cHandle, int cardIndex, int alsaDeviceIndex)
{
    snd_pcm_info_t* pcmInfo{ nullptr };
    snd_pcm_info_alloca(&pcmInfo);
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    int result = 0;

    snd_pcm_info_set_stream(pcmInfo, stream);

    std::stringstream name;
    if (cardIndex > -1) {
        name << "hw:" << cardIndex << "," << alsaDeviceIndex;
    } else {
        name << "default";
    }

    int openMode = SND_PCM_ASYNC;
    snd_pcm_t* pHandle{ nullptr };

    result = snd_pcm_open(&pHandle, name.str().c_str(), stream, openMode | SND_PCM_NONBLOCK);
    if (result < 0) {
        return 0;
    }

    result = snd_pcm_hw_params_any(pHandle, params);
    if (result < 0) {
        snd_pcm_close(pHandle);
        return 0;
    }

    unsigned int value{ 0 };
    result = snd_pcm_hw_params_get_channels_max(params, &value);
    snd_pcm_close(pHandle);

    return (result == 0 ? value : 0);
}


DeviceInfo Alsa::getCardDeviceInfo(int cardIndex, int deviceIndex)
{
    DeviceInfo deviceInfo{};
    snd_ctl_t* cHandle{ nullptr };

    int alsaDeviceIndex = this->getAlsaDeviceIndex(cardIndex, deviceIndex);

    std::stringstream name;
    if (cardIndex > -1) {
        name << "hw:" << cardIndex;
    } else {
        name << "default";
    }

    int result = snd_ctl_open(&cHandle, name.str().c_str(), SND_CTL_NONBLOCK);
    if (result != 0) {
        return deviceInfo;
    }

    deviceInfo.outputChannels = getDeviceOutputChannelCount(SND_PCM_STREAM_PLAYBACK, cHandle, cardIndex, alsaDeviceIndex);
    deviceInfo.inputChannels = getDeviceOutputChannelCount(SND_PCM_STREAM_CAPTURE, cHandle, cardIndex, alsaDeviceIndex);

    snd_ctl_close( cHandle );


    return deviceInfo;
}
}