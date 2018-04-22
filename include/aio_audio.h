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

#ifndef AUDIOCARD_IO_AUDIO_H
#define AUDIOCARD_IO_AUDIO_H

#include <type_traits>
#include <string>
#include <vector>

namespace acio {

enum class AudioFormat : int
{
    None = 0x00,
    SINT8 = 0x01,
    SINT16 = 0x02,
    SINT24 = 0x04,
    SINT32 = 0x08,
    FLOAT32 = 0x16,
    FLOAT64 = 0x64
};

inline AudioFormat operator & (AudioFormat lhs, AudioFormat rhs)

{
    return static_cast<AudioFormat>(static_cast<std::underlying_type_t<AudioFormat>>(lhs) &
                                    static_cast<std::underlying_type_t<AudioFormat>>(rhs));
}

inline AudioFormat operator | (AudioFormat lhs, AudioFormat rhs)

{
    return static_cast<AudioFormat>(static_cast<std::underlying_type_t<AudioFormat>>(lhs) |
                          static_cast<std::underlying_type_t<AudioFormat>>(rhs));
}

inline AudioFormat& operator |= (AudioFormat& lhs, AudioFormat rhs)
{
    lhs = static_cast<AudioFormat>(static_cast<std::underlying_type_t<AudioFormat>>(lhs) |
                                   static_cast<std::underlying_type_t<AudioFormat>>(rhs));
    return lhs;
}


struct DeviceInfo {
    std::string name;
    int outputChannels;
    int inputChannels;
    std::vector<unsigned int> sampleRates;
    unsigned int preferredSampleRate;
    AudioFormat nativeFormats;

    bool changeableInputChannelCount { false };
    bool changeableOutputChannelCount { false };
};

class Audio {
public:
    virtual ~Audio() = 0;

public:
    virtual int countDevices() = 0;
    virtual DeviceInfo * getDeviceInfo(int index) = 0;
};

inline Audio::~Audio() = default;
}

#endif //AUDIOCARD_IO_AUDIO_H
