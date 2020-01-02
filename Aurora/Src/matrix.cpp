#include <stdlib.h>
#include "Aurora/matrix.h"

Matrix::Matrix(int width, int height)
{
    MATRIX_WIDTH = width;
    MATRIX_HEIGHT = height;
    NUM_LEDS = 1;
    NUMMATRIX = MATRIX_WIDTH * MATRIX_HEIGHT;

    MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;
    MATRIX_CENTER_X = MATRIX_WIDTH / 2;

    MATRIX_CENTRE_X = MATRIX_CENTER_X - 1;
    MATRIX_CENTRE_Y = MATRIX_CENTER_Y - 1;

    framebuffer = (CRGB*)malloc(NUMMATRIX * sizeof(CRGB));
}
