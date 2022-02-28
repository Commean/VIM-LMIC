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
static u1_t APPEUI[8]={ };
void os_getArtEui (u1_t* buf) { memcpy(buf, APPEUI, 8);}
void os_setArtEui (u1_t* buf) { memcpy(APPEUI, buf, 8);}

// This should also be in little endian format, see above.
static u1_t DEVEUI[8]={ };
void os_getDevEui (u1_t* buf) { memcpy(buf, DEVEUI, 8);}
void os_setDevEui (u1_t* buf) { memcpy(DEVEUI, buf, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static u1_t APPKEY[16] = { };
void os_getDevKey (u1_t* buf) {  memcpy(buf, APPKEY, 16);}
void os_setDevKey (u1_t* buf) {  memcpy(APPKEY, buf, 16);}
