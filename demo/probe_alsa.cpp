#include "linux/aio_alsa.h"
#include <iostream>

#include "demo_probe.h"

int main(int argc, char* argv[])
{
    Probe<acio::Alsa>();

    return 0;
}