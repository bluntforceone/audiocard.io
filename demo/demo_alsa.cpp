#include "linux/aio_alsa.h"
#include <iostream>

int main(int argc, char* argv[])
{
    acio::Alsa alsa;

    int deviceCount = alsa.countDevices();
    std::cout << "countDevices:" << deviceCount << std::endl;

    for (int index = 0; index < deviceCount; ++index) {
        auto deviceInfo = alsa.getDeviceInfo(index);
        std::cout << "--------------------" << std::endl;
        std::cout << "name:" << deviceInfo.name << std::endl;
        std::cout << "outputChannels:" << deviceInfo.outputChannels << std::endl;
        std::cout << "inputChannels:" << deviceInfo.inputChannels << std::endl;
        std::cout << "--------------------" << std::endl;
    }

    return 0;
}