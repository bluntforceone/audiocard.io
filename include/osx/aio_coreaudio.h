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

#ifndef AUDIOCARD_IO_COREAUDIO_H
#define AUDIOCARD_IO_COREAUDIO_H

#include "aio_audio.h"
#include "osx/corefoundation/cf_string.h"
#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>

namespace acio {

class CoreAudio : public Audio {
public:
    CoreAudio();
    ~CoreAudio() override = default;

public:
    int countDevices() override;
    DeviceInfo getDeviceInfo(int index) override;

private:
    std::vector<DeviceInfo> _deviceInfo;
    std::vector<AudioObjectID> _deviceIds;
    int _deviceCount{ 0 };
};
}

#endif //AUDIOCARD_IO_COREAUDIO_H
