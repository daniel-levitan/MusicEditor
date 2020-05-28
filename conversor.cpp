
#include "conversor.h"

int f_round(float f) {
        f += (3<<22);
        return *((int*)&f) - 0x4b400000;
}

