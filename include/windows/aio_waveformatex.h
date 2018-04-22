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
#ifndef AUDIOCARD_IO_WAVEFORMATEX_H
#define AUDIOCARD_IO_WAVEFORMATEX_H

#include <Windows.h>
#include <iostream>
#include <mmreg.h>

namespace acio {
class WaveFormatEx : public WAVEFORMATEXTENSIBLE {
public:
    WaveFormatEx()
    {
        this->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    }
    WaveFormatEx(const WAVEFORMATEXTENSIBLE* waveformatEx)
    {
        this->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        this->Format.nAvgBytesPerSec = waveformatEx->Format.nAvgBytesPerSec;
        this->Format.nBlockAlign = waveformatEx->Format.nBlockAlign;
        this->Format.nSamplesPerSec = waveformatEx->Format.nSamplesPerSec;
        this->Format.nChannels = waveformatEx->Format.nChannels;
        this->Format.wBitsPerSample = waveformatEx->Format.wBitsPerSample;
        this->Format.wFormatTag = waveformatEx->Format.wFormatTag;
        this->dwChannelMask = waveformatEx->dwChannelMask;
        this->Samples = waveformatEx->Samples;
        this->SubFormat = waveformatEx->SubFormat;
    }
    WaveFormatEx(WORD formatTag, WORD channels, DWORD sampleRate)
    {
        this->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        this->Format.nChannels = channels;
        this->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        switch (formatTag) {
        case WAVE_FORMAT_IEEE_FLOAT: {
            this->Format.wBitsPerSample = sizeof(float) * 8;
            this->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
            break;
        }
        case WAVE_FORMAT_PCM: {
            this->Format.wBitsPerSample = sizeof(short) * 8;
            this->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
            break;
        }
        default: {
            this->Format.wBitsPerSample = 0;
        }
        }

        this->Format.nSamplesPerSec = sampleRate;
        this->Format.nBlockAlign = this->Format.nChannels * (this->Format.wBitsPerSample / 8) ;
        this->Format.nAvgBytesPerSec = this->Format.nSamplesPerSec * this->Format.nBlockAlign;

        this->Samples.wSamplesPerBlock = this->Format.wBitsPerSample;
        this->dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
    }
};

inline std::ostream& operator<<(std::ostream& os, const WAVEFORMATEXTENSIBLE& wfx)
{
    std::cout << "nAvgBytesPerSec = " << wfx.Format.nAvgBytesPerSec << "\n";
    std::cout << "nBlockAlign = " << wfx.Format.nBlockAlign << "\n";
    std::cout << "nSamplesPerSec = " << wfx.Format.nSamplesPerSec << "\n";
    std::cout << "nChannels = " << wfx.Format.nChannels << "\n";
    std::cout << "wBitsPerSample = " << wfx.Format.wBitsPerSample << "\n";
    std::cout << "wFormatTag = " << wfx.Format.wFormatTag << "\n";
    std::cout << "dwChannelMask = " << wfx.dwChannelMask << "\n";
    std::cout << "wSamplesPerBlock = " << wfx.Samples.wSamplesPerBlock << "\n";
    return os;
}
}

#endif //AUDIOCARD_IO_WAVEFORMATEX_H
