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

#ifndef AUDIOCARD_IO_ALSA_PCM_H
#define AUDIOCARD_IO_ALSA_PCM_H

#include "actl/aio_unique_ptr.h"
#include <alsa/asoundlib.h>

typedef struct _snd_ctl snd_ctl_t;

namespace acio {

class SndPcm : public custom_unique_ptr<snd_pcm_t> {
public:
    SndPcm(const std::string& name, snd_pcm_stream_t stream, int mode)
        : custom_unique_ptr<snd_pcm_t>(nullptr, [](snd_pcm_t* p) {
            snd_pcm_close(p);
        })
    {
        snd_pcm_t* handle{ nullptr };
        if (snd_pcm_open(&handle, name.c_str(), stream, mode) != 0) {
            return;
        }

        this->reset(handle);
    }

    snd_pcm_t* operator&()
    {
        return this->get();
    }
};
}
#endif //AUDIOCARD_IO_ALSA_PCM_H
