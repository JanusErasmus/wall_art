#ifndef __INC_LIB8TION_TRIG_H
#define __INC_LIB8TION_TRIG_H

///@ingroup lib8tion



/// Fast 16-bit approximation of cos(x). This approximation never varies more than
/// 0.69% from the floating point value you'd get by doing
///
///     float s = cos(x) * 32767.0;
///
/// @param theta input angle from 0-65535
/// @returns sin of theta, value between -32767 to 32767.
int16_t cos16( uint16_t theta);
///////////////////////////////////////////////////////////////////////

// sin8 & cos8
//        Fast 8-bit approximations of sin(x) & cos(x).
//        Input angle is an unsigned int from 0-255.
//        Output is an unsigned int from 0 to 255.
//
//        This approximation can vary to to 2%
//        from the floating point value you'd get by doing
//          float s = (sin( x ) * 128.0) + 128;
//
//        Don't use this approximation for calculating the
//        "real" trigonometric calculations, but it's great
//        for art projects and LED displays.
//
//        On Arduino/AVR, this approximation is more than
//        20X faster than floating point sin(x) and cos(x)



#if defined(__AVR__)
#define sin16 sin16_avr
#else
#define sin16 sin16_C
#endif
/// Fast 16-bit approximation of sin(x). This approximation never varies more than
/// 0.69% from the floating point value you'd get by doing
///
///     float s = sin(x) * 32767.0;
///
/// @param theta input angle from 0-65535
/// @returns sin of theta, value between -32767 to 32767.
int16_t sin16( uint16_t theta );


#if defined(__AVR__) && !defined(LIB8_ATTINY)
#define sin8 sin8_avr
#else
#define sin8 sin8_C
#endif
uint8_t sin8( uint8_t theta);

/// Fast 8-bit approximation of cos(x). This approximation never varies more than
/// 2% from the floating point value you'd get by doing
///
///     float s = (cos(x) * 128.0) + 128;
///
/// @param theta input angle from 0-255
/// @returns sin of theta, value between 0 and 255
uint8_t cos8( uint8_t theta);

///@}
#endif
