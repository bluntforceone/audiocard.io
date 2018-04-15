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
