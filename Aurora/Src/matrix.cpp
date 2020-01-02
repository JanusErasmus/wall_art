#include "Aurora/matrix.h"



Matrix::Matrix()
{
    MATRIX_WIDTH = 16;
    MATRIX_HEIGHT = 16;
    NUM_LEDS = 16;
    NUMMATRIX = MATRIX_WIDTH * MATRIX_HEIGHT;

    MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;
    MATRIX_CENTER_X = MATRIX_WIDTH / 2;

    MATRIX_CENTRE_X = MATRIX_CENTER_X - 1;
    MATRIX_CENTRE_Y = MATRIX_CENTER_Y - 1;

}
