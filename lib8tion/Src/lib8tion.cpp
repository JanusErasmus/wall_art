#include "lib8tion.h"

///////////////////////////////////////////////////////////////////////
//
// float-to-fixed and fixed-to-float conversions
//
// Note that anything involving a 'float' on AVR will be slower.

/// sfract15ToFloat: conversion from sfract15 fixed point to
///                  IEEE754 32-bit float.
LIB8STATIC float sfract15ToFloat( sfract15 y)
{
    return y / 32768.0;
}

/// conversion from IEEE754 float in the range (-1,1)
///                  to 16-bit fixed point.  Note that the extremes of
///                  one and negative one are NOT representable.  The
///                  representable range is basically
LIB8STATIC sfract15 floatToSfract15( float f)
{
    return f * 32768.0;
}



///////////////////////////////////////////////////////////////////////
//
// memmove8, memcpy8, and memset8:
//   alternatives to memmove, memcpy, and memset that are
//   faster on AVR than standard avr-libc 1.8

#if defined(__AVR__)
extern "C" {
void * memmove8( void * dst, const void * src, uint16_t num );
void * memcpy8 ( void * dst, const void * src, uint16_t num )  __attribute__ ((noinline));
void * memset8 ( void * ptr, uint8_t value, uint16_t num ) __attribute__ ((noinline)) ;
}
#else
// on non-AVR platforms, these names just call standard libc.
#define memmove8 memmove
#define memcpy8 memcpy
#define memset8 memset
#endif


///////////////////////////////////////////////////////////////////////
//
// linear interpolation, such as could be used for Perlin noise, etc.
//

// A note on the structure of the lerp functions:
// The cases for b>a and b<=a are handled separately for
// speed: without knowing the relative order of a and b,
// the value (a-b) might be overflow the width of a or b,
// and have to be promoted to a wider, slower type.
// To avoid that, we separate the two cases, and are able
// to do all the math in the same width as the arguments,
// which is much faster and smaller on AVR.

/// linear interpolation between two unsigned 8-bit values,
/// with 8-bit fraction
LIB8STATIC uint8_t lerp8by8( uint8_t a, uint8_t b, fract8 frac)
{
    uint8_t result;
    if( b > a) {
        uint8_t delta = b - a;
        uint8_t scaled = scale8( delta, frac);
        result = a + scaled;
    } else {
        uint8_t delta = a - b;
        uint8_t scaled = scale8( delta, frac);
        result = a - scaled;
    }
    return result;
}

/// linear interpolation between two unsigned 16-bit values,
/// with 16-bit fraction
LIB8STATIC uint16_t lerp16by16( uint16_t a, uint16_t b, fract16 frac)
{
    uint16_t result;
    if( b > a ) {
        uint16_t delta = b - a;
        uint16_t scaled = scale16(delta, frac);
        result = a + scaled;
    } else {
        uint16_t delta = a - b;
        uint16_t scaled = scale16( delta, frac);
        result = a - scaled;
    }
    return result;
}

/// linear interpolation between two unsigned 16-bit values,
/// with 8-bit fraction
LIB8STATIC uint16_t lerp16by8( uint16_t a, uint16_t b, fract8 frac)
{
    uint16_t result;
    if( b > a) {
        uint16_t delta = b - a;
        uint16_t scaled = scale16by8( delta, frac);
        result = a + scaled;
    } else {
        uint16_t delta = a - b;
        uint16_t scaled = scale16by8( delta, frac);
        result = a - scaled;
    }
    return result;
}

/// linear interpolation between two signed 15-bit values,
/// with 8-bit fraction
LIB8STATIC int16_t lerp15by8( int16_t a, int16_t b, fract8 frac)
{
    int16_t result;
    if( b > a) {
        uint16_t delta = b - a;
        uint16_t scaled = scale16by8( delta, frac);
        result = a + scaled;
    } else {
        uint16_t delta = a - b;
        uint16_t scaled = scale16by8( delta, frac);
        result = a - scaled;
    }
    return result;
}

/// linear interpolation between two signed 15-bit values,
/// with 8-bit fraction
LIB8STATIC int16_t lerp15by16( int16_t a, int16_t b, fract16 frac)
{
    int16_t result;
    if( b > a) {
        uint16_t delta = b - a;
        uint16_t scaled = scale16( delta, frac);
        result = a + scaled;
    } else {
        uint16_t delta = a - b;
        uint16_t scaled = scale16( delta, frac);
        result = a - scaled;
    }
    return result;
}

///  map8: map from one full-range 8-bit value into a narrower
/// range of 8-bit values, possibly a range of hues.
///
/// E.g. map myValue into a hue in the range blue..purple..pink..red
/// hue = map8( myValue, HUE_BLUE, HUE_RED);
///
/// Combines nicely with the waveform functions (like sin8, etc)
/// to produce continuous hue gradients back and forth:
///
///          hue = map8( sin8( myValue), HUE_BLUE, HUE_RED);
///
/// Mathematically simiar to lerp8by8, but arguments are more
/// like Arduino's "map"; this function is similar to
///
///          map( in, 0, 255, rangeStart, rangeEnd)
///
/// but faster and specifically designed for 8-bit values.
LIB8STATIC uint8_t map8( uint8_t in, uint8_t rangeStart, uint8_t rangeEnd)
{
    uint8_t rangeWidth = rangeEnd - rangeStart;
    uint8_t out = scale8( in, rangeWidth);
    out += rangeStart;
    return out;
}


///////////////////////////////////////////////////////////////////////
//
// easing functions; see http://easings.net
//

/// ease8InOutQuad: 8-bit quadratic ease-in / ease-out function
///                Takes around 13 cycles on AVR
#if EASE8_C == 1
LIB8STATIC uint8_t ease8InOutQuad( uint8_t i)
{
    uint8_t j = i;
    if( j & 0x80 ) {
        j = 255 - j;
    }
    uint8_t jj  = scale8(  j, j);
    uint8_t jj2 = jj << 1;
    if( i & 0x80 ) {
        jj2 = 255 - jj2;
    }
    return jj2;
}

#elif EASE8_AVRASM == 1
// This AVR asm version of ease8InOutQuad preserves one more
// low-bit of precision than the C version, and is also slightly
// smaller and faster.
LIB8STATIC uint8_t ease8InOutQuad(uint8_t val) {
    uint8_t j=val;
    asm volatile (
      "sbrc %[val], 7 \n"
      "com %[j]       \n"
      "mul %[j], %[j] \n"
      "add r0, %[j]   \n"
      "ldi %[j], 0    \n"
      "adc %[j], r1   \n"
      "lsl r0         \n" // carry = high bit of low byte of mul product
      "rol %[j]       \n" // j = (j * 2) + carry // preserve add'l bit of precision
      "sbrc %[val], 7 \n"
      "com %[j]       \n"
      "clr __zero_reg__   \n"
      : [j] "+&a" (j)
      : [val] "a" (val)
      : "r0", "r1"
      );
    return j;
}

#else
#error "No implementation for ease8InOutQuad available."
#endif

/// ease16InOutQuad: 16-bit quadratic ease-in / ease-out function
// C implementation at this point
LIB8STATIC uint16_t ease16InOutQuad( uint16_t i)
{
    uint16_t j = i;
    if( j & 0x8000 ) {
        j = 65535 - j;
    }
    uint16_t jj  = scale16( j, j);
    uint16_t jj2 = jj << 1;
    if( i & 0x8000 ) {
        jj2 = 65535 - jj2;
    }
    return jj2;
}


/// ease8InOutCubic: 8-bit cubic ease-in / ease-out function
///                 Takes around 18 cycles on AVR
LIB8STATIC fract8 ease8InOutCubic( fract8 i)
{
    uint8_t ii  = scale8_LEAVING_R1_DIRTY(  i, i);
    uint8_t iii = scale8_LEAVING_R1_DIRTY( ii, i);

    uint16_t r1 = (3 * (uint16_t)(ii)) - ( 2 * (uint16_t)(iii));

    /* the code generated for the above *'s automatically
       cleans up R1, so there's no need to explicitily call
       cleanup_R1(); */

    uint8_t result = r1;

    // if we got "256", return 255:
    if( r1 & 0x100 ) {
        result = 255;
    }
    return result;
}

/// ease8InOutApprox: fast, rough 8-bit ease-in/ease-out function
///                   shaped approximately like 'ease8InOutCubic',
///                   it's never off by more than a couple of percent
///                   from the actual cubic S-curve, and it executes
///                   more than twice as fast.  Use when the cycles
///                   are more important than visual smoothness.
///                   Asm version takes around 7 cycles on AVR.

#if EASE8_C == 1
LIB8STATIC fract8 ease8InOutApprox( fract8 i)
{
    if( i < 64) {
        // start with slope 0.5
        i /= 2;
    } else if( i > (255 - 64)) {
        // end with slope 0.5
        i = 255 - i;
        i /= 2;
        i = 255 - i;
    } else {
        // in the middle, use slope 192/128 = 1.5
        i -= 64;
        i += (i / 2);
        i += 32;
    }

    return i;
}

#elif EASE8_AVRASM == 1
LIB8STATIC uint8_t ease8InOutApprox( fract8 i)
{
    // takes around 7 cycles on AVR
    asm volatile (
        "  subi %[i], 64         \n\t"
        "  cpi  %[i], 128        \n\t"
        "  brcc Lshift_%=        \n\t"

        // middle case
        "  mov __tmp_reg__, %[i] \n\t"
        "  lsr __tmp_reg__       \n\t"
        "  add %[i], __tmp_reg__ \n\t"
        "  subi %[i], 224        \n\t"
        "  rjmp Ldone_%=         \n\t"

        // start or end case
        "Lshift_%=:              \n\t"
        "  lsr %[i]              \n\t"
        "  subi %[i], 96         \n\t"

        "Ldone_%=:               \n\t"

        : [i] "+&a" (i)
        :
        : "r0", "r1"
        );
    return i;
}
#else
#error "No implementation for ease8 available."
#endif



/// triwave8: triangle (sawtooth) wave generator.  Useful for
///           turning a one-byte ever-increasing value into a
///           one-byte value that oscillates up and down.
///
///           input         output
///           0..127        0..254 (positive slope)
///           128..255      254..0 (negative slope)
///
/// On AVR this function takes just three cycles.
///
LIB8STATIC uint8_t triwave8(uint8_t in)
{
    if( in & 0x80) {
        in = 255 - in;
    }
    uint8_t out = in << 1;
    return out;
}


// quadwave8 and cubicwave8: S-shaped wave generators (like 'sine').
//           Useful for turning a one-byte 'counter' value into a
//           one-byte oscillating value that moves smoothly up and down,
//           with an 'acceleration' and 'deceleration' curve.
//
//           These are even faster than 'sin8', and have
//           slightly different curve shapes.
//

/// quadwave8: quadratic waveform generator.  Spends just a little more
///            time at the limits than 'sine' does.
LIB8STATIC uint8_t quadwave8(uint8_t in)
{
    return ease8InOutQuad( triwave8( in));
}

/// cubicwave8: cubic waveform generator.  Spends visibly more time
///             at the limits than 'sine' does.
LIB8STATIC uint8_t cubicwave8(uint8_t in)
{
    return ease8InOutCubic( triwave8( in));
}

/// squarewave8: square wave generator.  Useful for
///           turning a one-byte ever-increasing value
///           into a one-byte value that is either 0 or 255.
///           The width of the output 'pulse' is
///           determined by the pulsewidth argument:
///
///~~~
///           If pulsewidth is 255, output is always 255.
///           If pulsewidth < 255, then
///             if input < pulsewidth  then output is 255
///             if input >= pulsewidth then output is 0
///~~~
///
/// the output looking like:
///
///~~~
///     255   +--pulsewidth--+
///      .    |              |
///      0    0              +--------(256-pulsewidth)--------
///~~~
///
/// @param in
/// @param pulsewidth
/// @returns square wave output
LIB8STATIC uint8_t squarewave8( uint8_t in, uint8_t pulsewidth)
{
    if( in < pulsewidth || (pulsewidth == 255)) {
        return 255;
    } else {
        return 0;
    }
}


/// A 4.4 integer (4 bits integer, 4 bits fraction)
typedef q<uint8_t, 4,4> q44;
/// A 6.2 integer (6 bits integer, 2 bits fraction)
typedef q<uint8_t, 6,2> q62;
/// A 8.8 integer (8 bits integer, 8 bits fraction)
typedef q<uint16_t, 8,8> q88;
/// A 12.4 integer (12 bits integer, 4 bits fraction)
typedef q<uint16_t, 12,4> q124;



// Beat generators - These functions produce waves at a given
//                   number of 'beats per minute'.  Internally, they use
//                   the Arduino function 'millis' to track elapsed time.
//                   Accuracy is a bit better than one part in a thousand.
//
//       beat8( BPM ) returns an 8-bit value that cycles 'BPM' times
//                    per minute, rising from 0 to 255, resetting to zero,
//                    rising up again, etc..  The output of this function
//                    is suitable for feeding directly into sin8, and cos8,
//                    triwave8, quadwave8, and cubicwave8.
//       beat16( BPM ) returns a 16-bit value that cycles 'BPM' times
//                    per minute, rising from 0 to 65535, resetting to zero,
//                    rising up again, etc.  The output of this function is
//                    suitable for feeding directly into sin16 and cos16.
//       beat88( BPM88) is the same as beat16, except that the BPM88 argument
//                    MUST be in Q8.8 fixed point format, e.g. 120BPM must
//                    be specified as 120*256 = 30720.
//       beatsin8( BPM, uint8_t low, uint8_t high) returns an 8-bit value that
//                    rises and falls in a sine wave, 'BPM' times per minute,
//                    between the values of 'low' and 'high'.
//       beatsin16( BPM, uint16_t low, uint16_t high) returns a 16-bit value
//                    that rises and falls in a sine wave, 'BPM' times per
//                    minute, between the values of 'low' and 'high'.
//       beatsin88( BPM88, ...) is the same as beatsin16, except that the
//                    BPM88 argument MUST be in Q8.8 fixed point format,
//                    e.g. 120BPM must be specified as 120*256 = 30720.
//
//  BPM can be supplied two ways.  The simpler way of specifying BPM is as
//  a simple 8-bit integer from 1-255, (e.g., "120").
//  The more sophisticated way of specifying BPM allows for fractional
//  "Q8.8" fixed point number (an 'accum88') with an 8-bit integer part and
//  an 8-bit fractional part.  The easiest way to construct this is to multiply
//  a floating point BPM value (e.g. 120.3) by 256, (e.g. resulting in 30796
//  in this case), and pass that as the 16-bit BPM argument.
//  "BPM88" MUST always be specified in Q8.8 format.
//
//  Originally designed to make an entire animation project pulse with brightness.
//  For that effect, add this line just above your existing call to "FastLED.show()":
//
//     uint8_t bright = beatsin8( 60 /*BPM*/, 192 /*dimmest*/, 255 /*brightest*/ ));
//     FastLED.setBrightness( bright );
//     FastLED.show();
//
//  The entire animation will now pulse between brightness 192 and 255 once per second.


// The beat generators need access to a millisecond counter.
// On Arduino, this is "millis()".  On other platforms, you'll
// need to provide a function with this signature:
//   uint32_t get_millisecond_timer();
// that provides similar functionality.
// You can also force use of the get_millisecond_timer function
// by #defining USE_GET_MILLISECOND_TIMER.
#if (defined(ARDUINO) || defined(SPARK) || defined(FASTLED_HAS_MILLIS)) && !defined(USE_GET_MILLISECOND_TIMER)
// Forward declaration of Arduino function 'millis'.
//uint32_t millis();
#define GET_MILLIS millis
#else
extern "C" uint32_t HAL_GetTick(void);
uint32_t get_millisecond_timer();
#define GET_MILLIS(_x) HAL_GetTick()
#endif

// beat16 generates a 16-bit 'sawtooth' wave at a given BPM,
///        with BPM specified in Q8.8 fixed-point format; e.g.
///        for this function, 120 BPM MUST BE specified as
///        120*256 = 30720.
///        If you just want to specify "120", use beat16 or beat8.
LIB8STATIC uint16_t beat88( accum88 beats_per_minute_88, uint32_t timebase)
{
    // BPM is 'beats per minute', or 'beats per 60000ms'.
    // To avoid using the (slower) division operator, we
    // want to convert 'beats per 60000ms' to 'beats per 65536ms',
    // and then use a simple, fast bit-shift to divide by 65536.
    //
    // The ratio 65536:60000 is 279.620266667:256; we'll call it 280:256.
    // The conversion is accurate to about 0.05%, more or less,
    // e.g. if you ask for "120 BPM", you'll get about "119.93".
    return (((GET_MILLIS()) - timebase) * beats_per_minute_88 * 280) >> 16;
}

/// beat16 generates a 16-bit 'sawtooth' wave at a given BPM
LIB8STATIC uint16_t beat16( accum88 beats_per_minute, uint32_t timebase)
{
    // Convert simple 8-bit BPM's to full Q8.8 accum88's if needed
    if( beats_per_minute < 256) beats_per_minute <<= 8;
    return beat88(beats_per_minute, timebase);
}

/// beat8 generates an 8-bit 'sawtooth' wave at a given BPM
LIB8STATIC uint8_t beat8( accum88 beats_per_minute, uint32_t timebase)
{
    return beat16( beats_per_minute, timebase) >> 8;
}

/// beatsin88 generates a 16-bit sine wave at a given BPM,
///           that oscillates within a given range.
///           For this function, BPM MUST BE SPECIFIED as
///           a Q8.8 fixed-point value; e.g. 120BPM must be
///           specified as 120*256 = 30720.
///           If you just want to specify "120", use beatsin16 or beatsin8.
LIB8STATIC uint16_t beatsin88( accum88 beats_per_minute_88, uint16_t lowest, uint16_t highest,
                              uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat88( beats_per_minute_88, timebase);
    uint16_t beatsin = (sin16( beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16( beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

/// beatsin16 generates a 16-bit sine wave at a given BPM,
///           that oscillates within a given range.
LIB8STATIC uint16_t beatsin16( accum88 beats_per_minute, uint16_t lowest, uint16_t highest,
                               uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat16( beats_per_minute, timebase);
    uint16_t beatsin = (sin16( beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16( beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

/// beatsin8 generates an 8-bit sine wave at a given BPM,
///           that oscillates within a given range.
LIB8STATIC uint8_t beatsin8( accum88 beats_per_minute, uint8_t lowest, uint8_t highest,
                            uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat8( beats_per_minute, timebase);
    uint8_t beatsin = sin8( beat + phase_offset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8( beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}


//uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest, uint32_t timebase, uint8_t phase_offset)
uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest, uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat8(beats_per_minute, timebase);
    uint8_t beatcos = cos8(beat + phase_offset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatcos, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

//uint8_t beattriwave8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest, uint32_t timebase, uint8_t phase_offset)
uint8_t beattriwave8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest, uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat8(beats_per_minute, timebase);
    uint8_t beatcos = triwave8(beat + phase_offset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatcos, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

uint8_t mapsin8(uint8_t theta, uint8_t lowest, uint8_t highest)
//uint8_t mapsin8(uint8_t theta, uint8_t lowest, uint8_t highest) {
{
    uint8_t beatsin = sin8(theta);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest, uint8_t highest) {
//uint8_t mapcos8(uint8_t theta, uint8_t lowest, uint8_t highest) {
    uint8_t beatcos = cos8(theta);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatcos, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}


/// Return the current seconds since boot in a 16-bit value.  Used as part of the
/// "every N time-periods" mechanism
LIB8STATIC uint16_t seconds16()
{
    uint32_t ms = GET_MILLIS();
    uint16_t s16;
    s16 = ms / 1000;
    return s16;
}

/// Return the current minutes since boot in a 16-bit value.  Used as part of the
/// "every N time-periods" mechanism
LIB8STATIC uint16_t minutes16()
{
    uint32_t ms = GET_MILLIS();
    uint16_t m16;
    m16 = (ms / (60000L)) & 0xFFFF;
    return m16;
}

/// Return the current hours since boot in an 8-bit value.  Used as part of the
/// "every N time-periods" mechanism
LIB8STATIC uint8_t hours8()
{
    uint32_t ms = GET_MILLIS();
    uint8_t h8;
    h8 = (ms / (3600000L)) & 0xFF;
    return h8;
}


/// Helper routine to divide a 32-bit value by 1024, returning
/// only the low 16 bits. You'd think this would be just
///   result = (in32 >> 10) & 0xFFFF;
/// and on ARM, that's what you want and all is well.
/// But on AVR that code turns into a loop that executes
/// a four-byte shift ten times: 40 shifts in all, plus loop
/// overhead. This routine gets exactly the same result with
/// just six shifts (vs 40), and no loop overhead.
/// Used to convert millis to 'binary seconds' aka bseconds:
/// one bsecond == 1024 millis.
LIB8STATIC uint16_t div1024_32_16( uint32_t in32)
{
    uint16_t out16;
#if defined(__AVR__)
    asm volatile (
                  "  lsr %D[in]  \n\t"
                  "  ror %C[in]  \n\t"
                  "  ror %B[in]  \n\t"
                  "  lsr %D[in]  \n\t"
                  "  ror %C[in]  \n\t"
                  "  ror %B[in]  \n\t"
                  "  mov %B[out],%C[in] \n\t"
                  "  mov %A[out],%B[in] \n\t"
                  : [in] "+r" (in32),
                  [out] "=r" (out16)
                  );
#else
    out16 = (in32 >> 10) & 0xFFFF;
#endif
    return out16;
}

/// bseconds16 returns the current time-since-boot in
/// "binary seconds", which are actually 1024/1000 of a
/// second long.
LIB8STATIC uint16_t bseconds16()
{
    uint32_t ms = GET_MILLIS();
    uint16_t s16;
    s16 = div1024_32_16( ms);
    return s16;
}

