#include "catch.hpp"
#include "fakeit.hpp"
#include "linux/aio_alsa.h"
#include <alsa/asoundlib.h>

struct AlsaDevice {
    std::string name;
};

std::vector<std::vector<AlsaDevice>> card_device;

TEST_CASE("Alsa")
{
    acio::Alsa alsa;

    card_device.emplace_back(std::vector<AlsaDevice>{ { "Charlie" }, { "Freddie" }, { "Alfie" } });
    card_device.emplace_back(std::vector<AlsaDevice>{ { "Kiera" }, { "Francesca" } });

    CHECK(alsa.countDevices() == 6);
}

int snd_card_next(int* card)
{
    if (nullptr == card) {
        return -EINVAL;
    }
    (*card) = (*card) < 0 ? 0 : (*card) + 1;

    if ((*card) >= card_device.size()) {
        (*card) = -1;
    }

    return 0;
}

int snd_ctl_open(snd_ctl_t** ctl, const char* name, int mode)
{
    if (nullptr == ctl) {
        return -EINVAL;
    }

    std::string sName = name;

    if ((sName == "default") && (card_device.size() > 0)) {
        *ctl = reinterpret_cast<snd_ctl_t*>(-1);
        return 0;
    }

    auto prefix = sName.substr(0, 3);
    if (prefix != "hw:") {
        return -ENODEV;
    }

    intptr_t index = std::atoi(sName.substr(3).c_str());

    if (index < 0 || index >= card_device.size()) {
        return -ENODEV;
    }

    *ctl = reinterpret_cast<snd_ctl_t*>(1000 + index);

    return 0;
}

int snd_ctl_pcm_next_device(snd_ctl_t* ctl, int* device)
{
    if (nullptr == device || nullptr == ctl) {
        return -EINVAL;
    }

    intptr_t index = reinterpret_cast<intptr_t>(ctl);

    if (-1 == index) {
        return -ENXIO;
    }

    index -= 1000;

    if (index < 0 || index >= card_device.size()) {
        return -EINVAL;
    }

    (*device) = (*device) < 0 ? 0 : (*device) + 1;

    if ((*device) >= card_device[index].size()) {
        (*device) = -1;
    }

    return 0;
}

int snd_ctl_close(snd_ctl_t* ctl)
{
    if (nullptr == ctl) {
        return -EINVAL;
    }
    return 0;
}