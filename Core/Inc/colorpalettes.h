#ifndef __INC_COLORPALETTES_H
#define __INC_COLORPALETTES_H

#include "colorutils.h"

/// Cloudy color pallete
extern const CRGBPalette16 CloudColors_p;
/// Lava colors
extern const CRGBPalette16 LavaColors_p;
/// Ocean colors, blues and whites
extern const CRGBPalette16 OceanColors_p;
/// Forest colors, greens
extern const CRGBPalette16 ForestColors_p;

/// HSV Rainbow
extern const CRGBPalette16 RainbowColors_p;

#define RainbowStripesColors_p RainbowStripeColors_p
/// HSV Rainbow colors with alternatating stripes of black
extern const CRGBPalette16 RainbowStripeColors_p FL_PROGMEM;

/// HSV color ramp: blue purple ping red orange yellow (and back)
/// Basically, everything but the greens, which tend to make
/// people's skin look unhealthy.  This palette is good for
/// lighting at a club or party, where it'll be shining on people.
extern const CRGBPalette16 PartyColors_p FL_PROGMEM;

/// Approximate "black body radiation" palette, akin to
/// the FastLED 'HeatColor' function.
/// Recommend that you use values 0-240 rather than
/// the usual 0-255, as the last 15 colors will be
/// 'wrapping around' from the hot end to the cold end,
/// which looks wrong.
extern const CRGBPalette16 HeatColors_p FL_PROGMEM;


DECLARE_GRADIENT_PALETTE( Rainbow_gp);

///@}
#endif
