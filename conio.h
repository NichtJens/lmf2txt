#ifndef _FAKE_CONIO_H_
#define _FAKE_CONIO_H_

#include <stdio.h>
#include <assert.h>
#include <errno.h>

// from https://stackoverflow.com/a/1513215
int fopen_s(FILE **f, const char *name, const char *mode) {
    int ret = 0;
    assert(f);
    *f = fopen(name, mode);
    if (! *f)
        ret = errno;
    return ret;
}

#define fscanf_s fscanf

#endif
