#ifndef AUDIOCARD_IO_ALSA_H
#define AUDIOCARD_IO_ALSA_H

#include "aio_audio.h"

namespace acio {

class Alsa : public Audio {
public:
    ~Alsa() override = default;

public:
    int countDevices() override;
    DeviceInfo getDeviceInfo(int index) override;

private:
    int countCards();
    int countCardDevices(int cardIndex);
    int countAllSubDevices();
    bool hasDefault();
    int getAlsaDeviceIndex(int cardIndex, int deviceIndex);

    DeviceInfo getCardDeviceInfo(int cardIndex, int deviceIndex);

};
}

#endif //AUDIOCARD_IO_ALSA_H
