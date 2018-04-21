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

    this->_deviceCount = io_size / sizeof(AudioObjectID);
    this->_deviceIds.resize(this->_deviceCount);
    this->_deviceInfo.resize(this->_deviceCount);

    os_err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &prop_address, 0, NULL, &io_size, this->_deviceIds.data());
    if (os_err != 0) {
        std::cout << "Error:" << os_err << std::endl;
        return;
    }

    for (int deviceIndex = 0; deviceIndex < this->_deviceCount; ++deviceIndex) {
    

        cf::String deviceName;

        prop_address.mSelector = kAudioObjectPropertyName;
        prop_address.mScope = kAudioObjectPropertyScopeGlobal;
        prop_address.mElement = kAudioObjectPropertyElementMaster;
        io_size = sizeof(CFStringRef);

        os_err = AudioObjectGetPropertyData(this->_deviceIds[deviceIndex], &prop_address, 0, NULL, &io_size, &deviceName.stringRef);
        if (os_err != 0) {
            std::cout << "Error:" << os_err << std::endl;
            continue;
        }
        this->_deviceInfo[deviceIndex].name = deviceName.stdString();
    }
}

int CoreAudio::countDevices()
{
    return this->_deviceCount;
}

DeviceInfo CoreAudio::getDeviceInfo(int index)
{
    return this->_deviceInfo[index];
}
}