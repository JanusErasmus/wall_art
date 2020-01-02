#ifndef SRC_ANIMATE_DISCO_H_
#define SRC_ANIMATE_DISCO_H_
#include "Drawable.h"

class AnimateDisco : public Drawable
{
    uint32_t tick = 0;
    double radius;
    int red_row;
    int red_col;
    int green_row;
    int green_col;
    int blue_row;
    int blue_col;
    double scale;
    int red_delta;
    int green_delta;
    int green_c_delta;
    int blue_delta;

    void get_scale(int row, int col, int *r, int *g, int *b);

public:
    AnimateDisco();
    virtual ~AnimateDisco();

    unsigned int drawFrame(Effects *effects);
};

#endif /* SRC_ANIMATE_DISCO_H_ */
