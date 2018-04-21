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
#include "windows/com/aio_propvariant.h"
#include <Audioclient.h>
#include <Endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <iostream>

namespace acio {

Wasapi::Wasapi()
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
    std::vector<DeviceInfo>& devices = input ? this->inputDevices : this->outputDevices;

    UINT count = 0;
    pDevices->GetCount(&count);
    devices.resize(count);

    for (UINT index = 0; index < count; ++index) {
        acom::ICom<IMMDevice> pDevice;
        if (S_OK == pDevices->Item(index, &pDevice)) {
            acom::ICom<IPropertyStore> pPropertyStore;
            pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
            acom::PropVariant propertyName;
            pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyName);
            devices[index].name = string_cast<std::string>(propertyName->pwszVal);
            
            acom::ICom<IAudioEndpointVolume> pAudioEndpointVolume;
            if (S_OK == pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume)) {
                UINT channelCount{ 0 };
                pAudioEndpointVolume->GetChannelCount(&channelCount);
                if (input) {
                    devices[index].inputChannels = static_cast<int>(channelCount);
                } else {
                    devices[index].outputChannels = static_cast<int>(channelCount);
                }
            }
        }
    }
}

int Wasapi::countDevices()
{
    return static_cast<int>(this->inputDevices.size() + this->outputDevices.size());
}

DeviceInfo Wasapi::getDeviceInfo(int index)
{
    if (index >= 0 && index < this->inputDevices.size()) {
        return this->inputDevices[index];
    }
    return this->outputDevices[index - this->inputDevices.size()];
}
}