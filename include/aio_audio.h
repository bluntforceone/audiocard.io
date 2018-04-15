#ifndef AUDIOCARD_IO_AUDIO_H
#define AUDIOCARD_IO_AUDIO_H

#include <type_traits>
#include <string>
#include <vector>

namespace acio {

enum class AudioFormat : int
{
    None = 0x00,
    SINT8 = 0x01,
    SINT16 = 0x02,
    SINT24 = 0x04,
    SINT32 = 0x08,
    FLOAT32 = 0x16,
    FLOAT64 = 0x64
};

inline AudioFormat operator | (AudioFormat lhs, AudioFormat rhs)

{
    return static_cast<AudioFormat>(static_cast<std::underlying_type_t<AudioFormat>>(lhs) |
                          static_cast<std::underlying_type_t<AudioFormat>>(rhs));
}

inline AudioFormat& operator |= (AudioFormat& lhs, AudioFormat rhs)
{
    lhs = static_cast<AudioFormat>(static_cast<std::underlying_type_t<AudioFormat>>(lhs) |
                                   static_cast<std::underlying_type_t<AudioFormat>>(rhs));
    return lhs;
}


struct DeviceInfo {
    std::string name;
    int outputChannels;
    int inputChannels;
    int duplexChannels;
    bool isDefaultOutput;
    bool isDefaultInput;
    std::vector<unsigned int> sampleRates;
    unsigned int preferredSampleRate;
    AudioFormat nativeFormats;

    bool changeableInputChannelCount { false };
    bool changeableOutputChannelCount { false };
};

class Audio {
public:
    virtual ~Audio() = 0;

public:
    virtual int countDevices() = 0;
    virtual DeviceInfo getDeviceInfo(int index) = 0;
};

inline Audio::~Audio() = default;
}

#endif //AUDIOCARD_IO_AUDIO_H
