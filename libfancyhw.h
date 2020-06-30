#ifndef LIBFANCYHW_H
#define LIBFANCYHW_H

#include <stdlib.h>
#include <stdint.h>

// This is the fictitious hardware interfacing library

void fancyhw_init(unsigned int init_param)
{
    srand(init_param);
}

int16_t fancyhw_read_val(void)
{
    return (int16_t)rand();
}

#endif
