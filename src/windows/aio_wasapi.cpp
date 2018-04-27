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
#include "windows/aio_wasapi.h"
#include "windows/aio_string.h"
#include "windows/aio_waveformatex.h"
#include "windows/com/aio_propvariant.h"
#include <Endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <array>
#include <iostream>

namespace acio {

const int MAX_SAMPLE_RATES = 14;
const std::array<unsigned int, MAX_SAMPLE_RATES> SAMPLE_RATES{
    4000, 5512, 8000, 9600, 11025, 16000, 22050,
    32000, 44100, 48000, 88200, 96000, 176400, 192000
};

Wasapi::Wasapi()
{
    this->enumDevices();
}

void Wasapi::enumDevices()
{
    acom::ICom<IMMDeviceCollection> pDevices;
    acom::ICom<IMMDeviceEnumerator> deviceEnumerator(__uuidof(MMDeviceEnumerator));

    HRESULT hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
    if (hr != S_OK) {
        return;
    }

    this->enumDeviceEnumerator(pDevices.obj, false);

    hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pDevices);
    if (hr != S_OK) {
        return;
    }

    this->enumDeviceEnumerator(pDevices.obj, true);
}

void Wasapi::enumDeviceEnumerator(IMMDeviceCollection* pDevices, bool input)
{
    UINT count = 0;
    pDevices->GetCount(&count);

    for (UINT index = 0; index < count; ++index) {
        acom::ICom<IMMDevice> pDevice;
        if (S_OK == pDevices->Item(index, &pDevice)) {
            acom::ICom<IPropertyStore> pPropertyStore;
            pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
            acom::PropVariant propertyName;
            pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyName);
            auto deviceInfo = WasapiDeviceInfo();
            deviceInfo.name = string_cast<std::string>(propertyName->pwszVal);
            this->queryDevice(pDevice.obj, deviceInfo, input);
            this->_devices.emplace_back(deviceInfo);
        }
    }
}

bool Wasapi::supportsExclusive(IAudioClient* pAudioClient)
{
    WAVEFORMATEX* pFormat{ nullptr };
    if (S_OK != pAudioClient->GetMixFormat(&pFormat)) {
        return false;
    }

    HRESULT hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, pFormat, nullptr);

    CoTaskMemFree(pFormat);

    return (hr == S_OK);
}

void Wasapi::querySampleRates(IAudioClient* pAudioClient, WasapiDeviceInfo& deviceInfo)
{
    bool exclusive = deviceInfo.supportsExclusive;

    WAVEFORMATEXTENSIBLE* pFormat{ nullptr };
    if (S_OK != pAudioClient->GetMixFormat(reinterpret_cast<WAVEFORMATEX**>(&pFormat))) {
        return;
    }

    for (auto testSampleRate : SAMPLE_RATES) {
        WAVEFORMATEXTENSIBLE* pSupported{ nullptr };
        WaveFormatEx formatEx(WAVE_FORMAT_IEEE_FLOAT, pFormat->Format.nChannels, testSampleRate);

        HRESULT hr = pAudioClient->IsFormatSupported(exclusive ? AUDCLNT_SHAREMODE_EXCLUSIVE : AUDCLNT_SHAREMODE_SHARED,
            reinterpret_cast<WAVEFORMATEX*>(&formatEx),
            exclusive ? nullptr : reinterpret_cast<WAVEFORMATEX**>(&pSupported));
        if (!exclusive && pSupported != nullptr) {
            CoTaskMemFree(pSupported);
            pSupported = nullptr;
        }
        if (hr == S_OK) {
            deviceInfo.sampleRates.push_back(testSampleRate);
        }
    }

    CoTaskMemFree(pFormat);
}

void Wasapi::queryDevice(IMMDevice* pDevice, WasapiDeviceInfo& deviceInfo, bool input)
{
    acom::ICom<IAudioEndpointVolume> pAudioEndpointVolume;
    if (S_OK == pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&pAudioEndpointVolume))) {
        UINT channelCount{ 0 };
        pAudioEndpointVolume->GetChannelCount(&channelCount);
        if (input) {
            deviceInfo.inputChannels = static_cast<int>(channelCount);
        } else {
            deviceInfo.outputChannels = static_cast<int>(channelCount);
        }
    }
    acom::ICom<IAudioClient> pAudioClient;
    pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&pAudioClient));

    deviceInfo.supportsExclusive = this->supportsExclusive(pAudioClient.obj);
    this->querySampleRates(pAudioClient.obj, deviceInfo);
}

int Wasapi::countDevices()
{
    return static_cast<int>(this->_devices.size());
}

DeviceInfo* Wasapi::getDeviceInfo(int64_t deviceId)
{
    auto idMap = IntDeviceIdMap(deviceId);
    if (idMap.intIndex >= this->_devices.size()) {
        return nullptr;
    }
    return &this->_devices[idMap.intIndex];
}

std::vector<int64_t> Wasapi::deviceIds()
{
    std::vector<int64_t> ids;
    for (int index = 0; index < this->_devices.size(); ++index) {
        auto idMap = IntDeviceIdMap::fromInt(index);
        ids.emplace_back(idMap.deviceId);
    }
    return ids;
}
}