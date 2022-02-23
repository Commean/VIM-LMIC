#include "config.h"
#include "oslmic.h"
#include "hal.h"
#include "local_hal.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>


int fd;

// -----------------------------------------------------------------------------
// I/O

static void hal_io_init () {
    wiringPiSetup();
    pinMode(pins.nss, OUTPUT);
    pinMode(pins.rxtx, OUTPUT);
    pinMode(pins.rst, OUTPUT);
    pinMode(pins.dio[0], OUTPUT);
    pinMode(pins.dio[1], OUTPUT);
    pinMode(pins.dio[2], OUTPUT);
}

// val == 1  => tx 1
void hal_pin_rxtx (u1_t val) {
    digitalWrite(pins.rxtx, val);
}

// set radio RST pin to given value (or keep floating!)
void hal_pin_rst (u1_t val) {
    if(val == 0 || val == 1) { // drive pin
        pinMode(pins.rst, OUTPUT);
        digitalWrite(pins.rst, val);
//        digitalWrite(0, val==0?LOW:HIGH);
    } else { // keep pin floating
        pinMode(pins.rst, INPUT);
    }
}

static bool dio_states[NUM_DIO] = {0};

static void hal_io_check() {
    u1_t i;
    for (i = 0; i < NUM_DIO; ++i) {
        if (dio_states[i] != digitalRead(pins.dio[i])) {
            dio_states[i] = !dio_states[i];
            if (dio_states[i]) {
                radio_irq_handler(i);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SPI
//
static int spifd;

static void hal_spi_init () {
    spifd = wiringPiSPISetup(0, 25000000);
}

void hal_pin_nss (u1_t val) {
    digitalWrite(pins.nss, val);
}

// perform SPI transaction with radio
u1_t hal_spi (u1_t out) {
    u1_t res = wiringPiSPIDataRW(0, &out, 1);
    return out;
}


// -----------------------------------------------------------------------------
// TIME

struct timespec tstart={0,0};
static void hal_time_init () {
    int res=clock_gettime(CLOCK_MONOTONIC_RAW, &tstart);
    tstart.tv_nsec=0; //Makes difference calculations in hal_ticks() easier
}

u4_t hal_ticks (void) {
    // Because micros() is scaled down in this function, micros() will
    // overflow before the tick timer should, causing the tick timer to
    // miss a significant part of its values if not corrected. To fix
    // this, the "overflow" serves as an overflow area for the micros()
    // counter. It consists of three parts:
    //  - The US_PER_OSTICK upper bits are effectively an extension for
    //    the micros() counter and are added to the result of this
    //    function.
    //  - The next bit overlaps with the most significant bit of
    //    micros(). This is used to detect micros() overflows.
    //  - The remaining bits are always zero.
    //
    // By comparing the overlapping bit with the corresponding bit in
    // the micros() return value, overflows can be detected and the
    // upper bits are incremented. This is done using some clever
    // bitwise operations, to remove the need for comparisons and a
    // jumps, which should result in efficient code. By avoiding shifts
    // other than by multiples of 8 as much as possible, this is also
    // efficient on AVR (which only has 1-bit shifts).
    static uint8_t overflow = 0;

    // Scaled down timestamp. The top US_PER_OSTICK_EXPONENT bits are 0,
    // the others will be the lower bits of our return value.
    uint32_t scaled = micros() >> US_PER_OSTICK_EXPONENT;
    // Most significant byte of scaled
    uint8_t msb = scaled >> 24;
    // Mask pointing to the overlapping bit in msb and overflow.
    const uint8_t mask = (1 << (7 - US_PER_OSTICK_EXPONENT));
    // Update overflow. If the overlapping bit is different
    // between overflow and msb, it is added to the stored value,
    // so the overlapping bit becomes equal again and, if it changed
    // from 1 to 0, the upper bits are incremented.
    overflow += (msb ^ overflow) & mask;

    // Return the scaled value with the upper bits of stored added. The
    // overlapping bit will be equal and the lower bits will be 0, so
    // bitwise or is a no-op for them.
    u4_t ticks = scaled | ((uint32_t)overflow << 24);
    //fprintf(stderr, "hal_ticks()=%d\n", ticks);

    return ticks;
}

// Returns the number of ticks until time.
static u4_t delta_time(u4_t time) {
      u4_t t = hal_ticks( );
      s4_t d = time - t;
      //fprintf(stderr, "deltatime(%d)=%d (%d)\n", time, d, t);
      if (d<=5) { return 0; }
      else {
        return (u4_t)d;
      }
}

void hal_waitUntil (u4_t time) {
    u4_t now=hal_ticks();
    u4_t delta = delta_time(time);
    fprintf(stdout, "waitUntil(%d) delta=%d\n", time, delta);
    s4_t t=time-now;
    if (delta==0) return;
    if (t>0) { 
      //fprintf(stderr, "delay(%d)\n", t*US_PER_OSTICK/1000);
      delay(t*US_PER_OSTICK/1000);
      return;
    }
}

// check and rewind for target time
u1_t hal_checkTimer (u4_t time) {
    // No need to schedule wakeup, since we're not sleeping
//    fprintf(stderr, "hal_checkTimer(%d):%d (%d)\n", time,  delta_time(time), hal_ticks());
    return delta_time(time) <= 0;
}

static u8_t irqlevel = 0;

void IRQ0(void) {
//  fprintf(stderr, "IRQ0 %d\n", irqlevel);
  if (irqlevel==0) {
    radio_irq_handler(0);
    return;
  }
}

void IRQ1(void) {
  if (irqlevel==0){
    radio_irq_handler(1);
  }
}

void IRQ2(void) {
  if (irqlevel==0){
    radio_irq_handler(2);
  }
}

void hal_disableIRQs () {
//    cli();
    irqlevel++;
//    fprintf(stderr, "disableIRQs(%d)\n", irqlevel);
}

void hal_enableIRQs () {
    if(--irqlevel == 0) {
//      fprintf(stderr, "enableIRQs(%d)\n", irqlevel);
//        sei();

        // Instead of using proper interrupts (which are a bit tricky
        // and/or not available on all pins on AVR), just poll the pin
        // values. Since os_runloop disables and re-enables interrupts,
        // putting this here makes sure we check at least once every
        // loop.
        //
        // As an additional bonus, this prevents the can of worms that
        // we would otherwise get for running SPI transfers inside ISRs
        hal_io_check();
      }
  }

  void hal_sleep () {
      // Not implemented
  }

  void hal_failed (const char *file, u2_t line) {
    fprintf(stderr, "FAILURE\n");
    fprintf(stderr, "%s:%d\n",file, line);
    hal_disableIRQs();
    while(1);
}

void hal_init() {
    fd=wiringPiSetup();
    hal_io_init();
    // configure radio SPI
    hal_spi_init();
    // configure timer and interrupt handler
    hal_time_init();
    wiringPiISR(pins.dio[0], INT_EDGE_RISING, IRQ0);
    wiringPiISR(pins.dio[1], INT_EDGE_RISING, IRQ1);
    wiringPiISR(pins.dio[2], INT_EDGE_RISING, IRQ2);

  
}

