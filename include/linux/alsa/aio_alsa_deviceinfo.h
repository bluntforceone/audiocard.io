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

#ifndef AUDIOCARD_IO_ALSA_DEVICEINFO_H
#define AUDIOCARD_IO_ALSA_DEVICEINFO_H

#include "linux/alsa/aio_alsa_pcm.h"
#include <alsa/asoundlib.h>
#include <array>
#include <string>

inline int getDeviceChannelCount(snd_pcm_stream_t stream, const std::string& deviceId)
{
    snd_pcm_info_t* pcmInfo{ nullptr };
    snd_pcm_info_alloca(&pcmInfo);
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    int result = 0;

    snd_pcm_info_set_stream(pcmInfo, stream);

    SndPcm pHandle(deviceId, stream, SND_PCM_ASYNC | SND_PCM_NONBLOCK);
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

inline auto getDeviceSampleRates(snd_pcm_stream_t stream, const std::string& deviceId)
{
    constexpr std::array<unsigned int, 14> SAMPLE_RATES{
        4000, 5512, 8000, 9600, 11025, 16000, 22050,
        32000, 44100, 48000, 88200, 96000, 176400, 192000
    };

    auto sampleRates = std::vector<unsigned int>();

    SndPcm pHandle(deviceId, stream, SND_PCM_ASYNC | SND_PCM_NONBLOCK);

    if (!pHandle) {
        return sampleRates;
    }

    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);

    int result = snd_pcm_hw_params_any(&pHandle, params);
    if (result < 0) {
        return sampleRates;
    }

    for (auto& sampleRate : SAMPLE_RATES) {
        if (snd_pcm_hw_params_test_rate(&pHandle, params, sampleRate, 0) == 0) {
            sampleRates.push_back(sampleRate);
        }
    }

    return sampleRates;
}

#endif //AUDIOCARD_IO_ALSA_DEVICEINFO_H
