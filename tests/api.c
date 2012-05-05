/* Test runner for public API tests */
#include <stdio.h>
#include <stdlib.h>
#include "ureg.h"

int main(int argc, char **argv)
{
    ureg_regexp r;
    unsigned long ev;
    char *err = NULL;
    int res;

    if (argc < 4)
        exit(1);
    ev = strtoul(argv[3], &err, 10);
    if (err == NULL || err == argv[3] || *err != '\0' || ev > 1)
        exit(1);
    r = ureg_compile(argv[1], 0);
    if (r == NULL)
        exit(1);

    res = ureg_match(r, argv[2]) != ev;

    ureg_free(r);
    exit(res);
}
