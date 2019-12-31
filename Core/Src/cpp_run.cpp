#include "animate_disco.h"

AnimateDisco disco;

extern "C" {
void cpp_init()
{

}

void cpp_run()
{
    disco.run();
}
}
