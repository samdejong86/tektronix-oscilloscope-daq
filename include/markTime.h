#include <sys/time.h>

struct timeval timeMark;

//returns the time of the call.
double markTime() {
  gettimeofday(&timeMark,NULL);
  return (double)timeMark.tv_sec + (double)timeMark.tv_usec/1000000.; 
}
