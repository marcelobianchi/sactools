#include <stdio.h>

#include "geodesic.h"

float time2distance(float target);
int model_locate(float slon, float slat, float distance, float azimuth, float *elon, float *elat);
float distance2ptime(float distance);
float distance2stime(float distance);
