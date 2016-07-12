#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void parabola(float *lags, int n_lags, int index, float best_lag, float dt, float *xmax, float *ymax);


float *correl (float *tra, float *trb,
		int len_a, int len_b,
		float *max_coef,
		int *index, float *dif_coef,
		int *problem, int * n_index);
