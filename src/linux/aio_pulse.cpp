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

#include "linux/aio_pulse.h"
#include <array>

namespace acio {

namespace {
    std::array<int, 7> SupportedSampleRates{ 8000, 16000, 22050, 32000,
        44100, 48000, 96000 };
}

Pulse::Pulse()
{
    this->deviceInfo.name = "PulseAudio";
    this->deviceInfo.outputChannels = 2;
    this->deviceInfo.inputChannels = 2;

    this->deviceInfo.sampleRates = std::vector<unsigned int>(SupportedSampleRates.begin(), SupportedSampleRates.end());
    this->deviceInfo.preferredSampleRate = 48000;
    this->deviceInfo.nativeFormats = AudioFormat::SINT16 | AudioFormat::SINT32 | AudioFormat::FLOAT32;
}

int Pulse::countDevices()
{
    return 1;
}

DeviceInfo* Pulse::getDeviceInfo(int index)
{
    return &this->deviceInfo;
}
}