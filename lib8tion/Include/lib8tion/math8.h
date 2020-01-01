#ifndef __INC_LIB8TION_MATH_H
#define __INC_LIB8TION_MATH_H

#include "scale8.h"

///@ingroup lib8tion

///@defgroup Math Basic math operations
/// Fast, efficient 8-bit math functions specifically
/// designed for high-performance LED programming.
///
/// Because of the AVR(Arduino) and ARM assembly language
/// implementations provided, using these functions often
/// results in smaller and faster code than the equivalent
/// program using plain "C" arithmetic and logic.
///@{


/// add one byte to another, saturating at 0xFF
/// @param i - first byte to add
/// @param j - second byte to add
/// @returns the sum of i & j, capped at 0xFF
 uint8_t qadd8( uint8_t i, uint8_t j);

/// Add one byte to another, saturating at 0x7F
/// @param i - first byte to add
/// @param j - second byte to add
/// @returns the sum of i & j, capped at 0xFF
 int8_t qadd7( int8_t i, int8_t j);

/// subtract one byte from another, saturating at 0x00
/// @returns i - j with a floor of 0
 uint8_t qsub8( uint8_t i, uint8_t j);

/// add one byte to another, with one byte result
 uint8_t add8( uint8_t i, uint8_t j);

/// add one byte to another, with one byte result
 uint16_t add8to16( uint8_t i, uint16_t j);


/// subtract one byte from another, 8-bit result
 uint8_t sub8( uint8_t i, uint8_t j);

/// Calculate an integer average of two unsigned
///       8-bit integer values (uint8_t).
///       Fractional results are rounded down, e.g. avg8(20,41) = 30
 uint8_t avg8( uint8_t i, uint8_t j);

/// Calculate an integer average of two unsigned
///       16-bit integer values (uint16_t).
///       Fractional results are rounded down, e.g. avg16(20,41) = 30
 uint16_t avg16( uint16_t i, uint16_t j);


/// Calculate an integer average of two signed 7-bit
///       integers (int8_t)
///       If the first argument is even, result is rounded down.
///       If the first argument is odd, result is result up.
 int8_t avg7( int8_t i, int8_t j);

/// Calculate an integer average of two signed 15-bit
///       integers (int16_t)
///       If the first argument is even, result is rounded down.
///       If the first argument is odd, result is result up.
 int16_t avg15( int16_t i, int16_t j);


///       Calculate the remainder of one unsigned 8-bit
///       value divided by anoter, aka A % M.
///       Implemented by repeated subtraction, which is
///       very compact, and very fast if A is 'probably'
///       less than M.  If A is a large multiple of M,
///       the loop has to execute multiple times.  However,
///       even in that case, the loop is only two
///       instructions long on AVR, i.e., quick.
 uint8_t mod8( uint8_t a, uint8_t m);

///          Add two numbers, and calculate the modulo
///          of the sum and a third number, M.
///          In other words, it returns (A+B) % M.
///          It is designed as a compact mechanism for
///          incrementing a 'mode' switch and wrapping
///          around back to 'mode 0' when the switch
///          goes past the end of the available range.
///          e.g. if you have seven modes, this switches
///          to the next one and wraps around if needed:
///            mode = addmod8( mode, 1, 7);
///See 'mod8' for notes on performance.
 uint8_t addmod8( uint8_t a, uint8_t b, uint8_t m);

///          Subtract two numbers, and calculate the modulo
///          of the difference and a third number, M.
///          In other words, it returns (A-B) % M.
///          It is designed as a compact mechanism for
///          incrementing a 'mode' switch and wrapping
///          around back to 'mode 0' when the switch
///          goes past the end of the available range.
///          e.g. if you have seven modes, this switches
///          to the next one and wraps around if needed:
///            mode = addmod8( mode, 1, 7);
///See 'mod8' for notes on performance.
 uint8_t submod8( uint8_t a, uint8_t b, uint8_t m);

/// 8x8 bit multiplication, with 8 bit result
 uint8_t mul8( uint8_t i, uint8_t j);


/// saturating 8x8 bit multiplication, with 8 bit result
/// @returns the product of i * j, capping at 0xFF
 uint8_t qmul8( uint8_t i, uint8_t j);


/// take abs() of a signed 8-bit uint8_t
 int8_t abs8( int8_t i);

///         square root for 16-bit integers
///         About three times faster and five times smaller
///         than Arduino's general sqrt on AVR.
 uint8_t sqrt16(uint16_t x);

/// blend a variable proproportion(0-255) of one byte to another
/// @param a - the starting byte value
/// @param b - the byte value to blend toward
/// @param amountOfB - the proportion (0-255) of b to blend
/// @returns a byte value between a and b, inclusive
#if (FASTLED_BLEND_FIXED == 1)
 uint8_t blend8( uint8_t a, uint8_t b, uint8_t amountOfB);
#else
 uint8_t blend8( uint8_t a, uint8_t b, uint8_t amountOfB);
#endif


///@}
#endif
