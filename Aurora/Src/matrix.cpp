#include "Aurora/matrix.h"

Matrix::Matrix(FrameBuffer *frame_buffer) : frame_buffer(frame_buffer)
{
}

CRGB *Matrix::getPixel(int x, int  y)
{
    return &frame_buffer->buffer[x * y * frame_buffer->width];
}
