#include "amiga.h"
#include <sys/time.h>

static long long timeval2utime(struct timeval*t) {
        return (t->tv_sec * 1000LL * 1000LL) + t->tv_usec;
}

static long long getutime64(void) {
        struct timeval t;
        gettimeofday(&t, 0);
        return timeval2utime(&t);
}

int getmicros() {
//   ULONG Seconds, Micros;
//   CurrentTime(&Seconds,&Micros);
//   return((int) (Micros % 100) + 1);
   return getutime64()%100 + 1;
}


int gethundredths() {
//  ULONG Seconds, Micros;
//  CurrentTime(&Seconds,&Micros);
//  return((int)Micros/10000);
  return getutime64()/10000;
}

#if 1
float frand() {
  return ( (float) getmicros() / 101.0);
}
#else
float frand() {
	return (float)rand()/RAND_MAX;
}
#endif
