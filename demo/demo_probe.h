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

#ifndef AUDIOCARD_IO_DEMO_PROBE_H
#define AUDIOCARD_IO_DEMO_PROBE_H

#include <iostream>

template<typename T>
void Probe()
{
    T api;

    int deviceCount = api.countDevices();
    std::cout << "countDevices:" << deviceCount << std::endl;

    for (int index = 0; index < deviceCount; ++index) {
        auto deviceInfo = api.getDeviceInfo(index);
        std::cout << "--------------------" << std::endl;
        std::cout << "name:" << deviceInfo.name << std::endl;
        std::cout << "outputChannels:" << deviceInfo.outputChannels << std::endl;
        std::cout << "inputChannels:" << deviceInfo.inputChannels << std::endl;

        for (auto iter = deviceInfo.sampleRates.begin(); iter != deviceInfo.sampleRates.end(); iter++) {
            if (iter != deviceInfo.sampleRates.begin()) {
                std::cout << ", ";
            }
            std::cout << *iter;
        }
        std::cout << std::endl;
        std::cout << "--------------------" << std::endl;
    }
}

#endif //AUDIOCARD_IO_DEMO_PROBE_H
