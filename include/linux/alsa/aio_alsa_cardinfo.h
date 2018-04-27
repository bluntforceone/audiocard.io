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

#ifndef AUDIOCARD_IO_ALSA_CARDINFO_H
#define AUDIOCARD_IO_ALSA_CARDINFO_H


#include "linux/alsa/aio_alsa_deviceinfo.h"
#include "linux/alsa/aio_alsa_ctl.h"
#include "linux/alsa/aio_alsa_pcm.h"
#include <alsa/asoundlib.h>
#include <array>
#include <sstream>
#include <string>
#include <vector>

namespace acio {

inline int alsaCountCards() {
    int cardCount{0};
    int cardIndex{-1};

    while ((snd_card_next(&cardIndex) == 0) && (cardIndex != -1)) {

        SndCtl handle(alsaDeviceId(cardIndex).c_str(), 0);

        if (handle) {
            ++cardCount;
        }
    }

    return cardCount;
}

inline std::vector<int> alsaCardIds() {
    auto cardIds = std::vector<int>();

    int cardIndex{-1};

    while ((snd_card_next(&cardIndex) == 0) && (cardIndex != -1)) {

        SndCtl handle(alsaDeviceId(cardIndex).c_str(), 0);
        if (handle) {
            cardIds.emplace_back(cardIndex);
        }
    }

    return cardIds;
}

inline std::vector<int> alsaPcmDeviceIds(int cardId) {
    auto deviceIds = std::vector<int>();

    SndCtl handle(alsaDeviceId(cardId), 0);
    int deviceId{-1};

    while ((snd_ctl_pcm_next_device(&handle, &deviceId) == 0) && (deviceId != -1)) {
        deviceIds.emplace_back(deviceId);
    }

    return deviceIds;
}

inline std::vector<int> alsaMidiDeviceIds(int cardId) {
    auto deviceIds = std::vector<int>();

    SndCtl handle(alsaDeviceId(cardId), 0);
    int deviceId{-1};

    while ((snd_ctl_rawmidi_next_device(&handle, &deviceId) == 0) && (deviceId != -1)) {
        deviceIds.emplace_back(deviceId);
    }

    return deviceIds;
}
}

#endif //AUDIOCARD_IO_ALSA_CARDINFO_H
