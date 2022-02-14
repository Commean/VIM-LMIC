//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t APPEUI[8]={ 0x88, 0x8A, 0x24, 0x90, 0xF2, 0xF5, 0x24, 0x34 };
void os_getArtEui (u1_t* buf) { memcpy(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t DEVEUI[8]={ 0xCE, 0xC3, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70  };
void os_getDevEui (u1_t* buf) { memcpy(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t APPKEY[16] = { 0xBC, 0xC8, 0x80, 0x9E, 0x93, 0xC8, 0x65, 0xA4, 0x17, 0x7F, 0x07, 0x4E, 0x9E, 0x17, 0xEA, 0xAA };
void os_getDevKey (u1_t* buf) {  memcpy(buf, APPKEY, 16);}
