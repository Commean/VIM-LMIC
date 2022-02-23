# vim-lmic

This is lmic from IBM ported to Khadas VIM3.

Based on [ernstdevreede/lmic_pi](https://github.com/ernstdevreede/lmic_pi) (v2) and [mcci-catena/arduino-lmic](https://github.com/mcci-catena/arduino-lmic) (v1).


## Dependencies

WiringPi is required to run this application.

- [WiringPi from Khadas](https://github.com/khadas/WiringPi) (currently broken)
- [Fixed WiringPi for VIM3](https://github.com/2tefan/WiringPi)


## Installing

First the lmic-library has to be installed.

## With scripts

```
$ ./build.sh
# ./install.sh
```

## Manual

Create build directory:
```
mkdir src/build
cd src/build
```

Create build files and build:
```
cmake ..
make
```

Install on system:
```
sudo make install
```
