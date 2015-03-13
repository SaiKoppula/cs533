#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>

struct timeval tv; 
double get_clock() {
    struct timeval tv; int ok;
    ok = gettimeofday(&tv, (void *) 0);
    if (ok<0) { printf("gettimeofday error");  }
    return (tv.tv_sec * 1.0 + tv.tv_usec * 1.0E-6); 
}
