#include "Aurora/colorutils.h"
#include "Aurora/colorpalettes.h"

const CRGBPalette16 WoodFireColors_p      = CRGBPalette16(CRGB::Black, CRGB::OrangeRed, CRGB::Orange, CRGB::Gold);         //* Orange
const CRGBPalette16 SodiumFireColors_p    = CRGBPalette16(CRGB::Black, CRGB::Orange, CRGB::Gold, CRGB::Goldenrod);         //* Yellow
const CRGBPalette16 CopperFireColors_p    = CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::GreenYellow, CRGB::LimeGreen);   //* Green
const CRGBPalette16 AlcoholFireColors_p   = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::DeepSkyBlue, CRGB::LightSkyBlue); //* Blue
const CRGBPalette16 RubidiumFireColors_p  = CRGBPalette16(CRGB::Black, CRGB::Indigo, CRGB::Indigo, CRGB::DarkBlue);        //* Indigo
const CRGBPalette16 PotassiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Indigo, CRGB::MediumPurple, CRGB::DeepPink);  //* Violet
const CRGBPalette16 LithiumFireColors_p   = CRGBPalette16(CRGB::Black, CRGB::FireBrick, CRGB::Pink, CRGB::DeepPink);       //* Red
const CRGBPalette16 IceColors_p           = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);const CRGBPalette16 GrayColors_p = CRGBPalette16(CRGB::Black, CRGB::White);

const CRGBPalette16 CloudColors_p =
        CRGBPalette16(
    CRGB::Blue,
    CRGB::DarkBlue,
    CRGB::DarkBlue,
    CRGB::DarkBlue,

    CRGB::DarkBlue,
    CRGB::DarkBlue,
    CRGB::DarkBlue,
    CRGB::DarkBlue,

    CRGB::Blue,
    CRGB::DarkBlue,
    CRGB::SkyBlue,
    CRGB::SkyBlue,

    CRGB::LightBlue,
    CRGB::White,
    CRGB::LightBlue,
    CRGB::SkyBlue
);

const CRGBPalette16 LavaColors_p =
        CRGBPalette16(
    CRGB::Black,
    CRGB::Maroon,
    CRGB::Black,
    CRGB::Maroon,

    CRGB::DarkRed,
    CRGB::Maroon,
    CRGB::DarkRed,
    CRGB::DarkRed,

    CRGB::DarkRed,
    CRGB::DarkRed,
    CRGB::Red,
    CRGB::Orange,

    CRGB::White,
    CRGB::Orange,
    CRGB::Red,
    CRGB::DarkRed
);


const CRGBPalette16 OceanColors_p =
        CRGBPalette16(
    CRGB::MidnightBlue,
    CRGB::DarkBlue,
    CRGB::MidnightBlue,
    CRGB::Navy,

    CRGB::DarkBlue,
    CRGB::MediumBlue,
    CRGB::SeaGreen,
    CRGB::Teal,

    CRGB::CadetBlue,
    CRGB::Blue,
    CRGB::DarkCyan,
    CRGB::CornflowerBlue,

    CRGB::Aquamarine,
    CRGB::SeaGreen,
    CRGB::Aqua,
    CRGB::LightSkyBlue
);

const CRGBPalette16 ForestColors_p =
        CRGBPalette16(
    CRGB::DarkGreen,
    CRGB::DarkGreen,
    CRGB::DarkOliveGreen,
    CRGB::DarkGreen,

    CRGB::Green,
    CRGB::ForestGreen,
    CRGB::OliveDrab,
    CRGB::Green,

    CRGB::SeaGreen,
    CRGB::MediumAquamarine,
    CRGB::LimeGreen,
    CRGB::YellowGreen,

    CRGB::LightGreen,
    CRGB::LawnGreen,
    CRGB::MediumAquamarine,
    CRGB::ForestGreen
);

/// HSV Rainbow
const CRGBPalette16 RainbowColors_p =
        CRGBPalette16(
    CRGB(0xFF0000), CRGB(0xD52A00), CRGB(0xAB5500), CRGB(0xAB7F00),
    CRGB(0xABAB00), CRGB(0x56D500), CRGB(0x00FF00), CRGB(0x00D52A),
    CRGB(0x00AB55), CRGB(0x0056AA), CRGB(0x0000FF), CRGB(0x2A00D5),
    CRGB(0x5500AB), CRGB(0x7F0081), CRGB(0xAB0055), CRGB(0xD5002B)
);

/// HSV Rainbow colors with alternatating stripes of black
const CRGBPalette16 RainbowStripeColors_p =
        CRGBPalette16(
    CRGB(0xFF0000), CRGB(0x000000), CRGB(0xAB5500), CRGB(0x000000),
    CRGB(0xABAB00), CRGB(0x000000), CRGB(0x00FF00), CRGB(0x000000),
    CRGB(0x00AB55), CRGB(0x000000), CRGB(0x0000FF), CRGB(0x000000),
    CRGB(0x5500AB), CRGB(0x000000), CRGB(0xAB0055), CRGB(0x000000)
);

/// HSV color ramp: blue purple ping red orange yellow (and back)
/// Basically, everything but the greens, which tend to make
/// people's skin look unhealthy.  This palette is good for
/// lighting at a club or party, where it'll be shining on people.
const CRGBPalette16 PartyColors_p =
        CRGBPalette16(
    CRGB(0x5500AB), CRGB(0x84007C), CRGB(0xB5004B), CRGB(0xE5001B),
    CRGB(0xE81700), CRGB(0xB84700), CRGB(0xAB7700), CRGB(0xABAB00),
    CRGB(0xAB5500), CRGB(0xDD2200), CRGB(0xF2000E), CRGB(0xC2003E),
    CRGB(0x8F0071), CRGB(0x5F00A1), CRGB(0x2F00D0), CRGB(0x0007F9)
);

/// Approximate "black body radiation" palette, akin to
/// the FastLED 'HeatColor' function.
/// Recommend that you use values 0-240 rather than
/// the usual 0-255, as the last 15 colors will be
/// 'wrapping around' from the hot end to the cold end,
/// which looks wrong.
const CRGBPalette16 HeatColors_p FL_PROGMEM =
        CRGBPalette16(
    CRGB(0x000000),
    CRGB(0x330000), CRGB(0x660000), CRGB(0x990000), CRGB(0xCC0000), CRGB(0xFF0000),
    CRGB(0xFF3300), CRGB(0xFF6600), CRGB(0xFF9900), CRGB(0xFFCC00), CRGB(0xFFFF00),
    CRGB(0xFFFF33), CRGB(0xFFFF66), CRGB(0xFFFF99), CRGB(0xFFFFCC), CRGB(0xFFFFFF)
);
