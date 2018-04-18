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

#include "aio_asio.h"
#include <array>
#include <memory>

namespace acio
{

Asio::Asio()
{
    const int maxDrivers = 32;

    std::array<char*, maxDrivers> driverNamesPtr;
    std::array<std::array<char, 32>, maxDrivers> driverBuffers;
    for (int index = 0; index < maxDrivers; ++index)
    {
        driverNamesPtr[index] = driverBuffers[index].data();
    }

    auto driverCount = this->asioDrivers.getDriverNames(driverNamesPtr.data(), maxDrivers);
    for (long index = 0; index < driverCount; ++index)
    {
        this->driverNames.push_back(driverNamesPtr[index]);
    }
}

int Asio::countDevices()
{
    return this->asioDrivers.asioGetNumDev();
}

DeviceInfo Asio::getDeviceInfo(int index)
{
    DeviceInfo deviceInfo {};

    deviceInfo.name = this->driverNames[index];

    return deviceInfo;
}
}
