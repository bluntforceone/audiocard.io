# audiocard.io
Cross Platform Library for Linux, Windows and MacOS to talk to audio hardware

APIs which will be supported:
  * CoreAudio - OSx
  * WASAPI - Windows
  * ASIO - Windows
  * PulseAudio - Linux
  * Jack - Windows, Linux, OSx
  * Alsa - Linux

This library is currenly pre-alpha and under development. It is not currently functional. 

Pull requests are welcome:
  All contributions must be released to the project under the MIT license. 

audiocard is licensed under the MIT license. 


Credit where credit is due: This library is largely inspired by Gary P. Scavone's work on RtAudio. 

The goals for this project differ from Gary's:
  * Keep the different implementations in different source files and use conditions in the CMakeLists.txt file, rather than #ifdefs in a single cpp file. 
  * Use more modern C++ (C++17)
  
Neither of these goals are criticism of Gary's project, he started RtAudio long before modern C++ existed and to update it to modern C++ would potentially restrict the existing user base.
