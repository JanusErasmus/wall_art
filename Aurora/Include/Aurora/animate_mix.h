#ifndef SRC_ANIMATE_MIX_H_
#define SRC_ANIMATE_MIX_H_
#include <stdint.h>

class AnimateMix
{
    uint32_t tick = 0;
    int r;
    int g;
    int b;
    int row_delta;
    int mix_state;
    int mix_delta;
    int main_delta;

    void column_run(int *row);

public:
    AnimateMix();
    virtual ~AnimateMix();

    void run();
};

#endif /* SRC_ANIMATE_MIX_H_ */
