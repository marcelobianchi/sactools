#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float *correl (float *tra, float *trb,
		int len_a, int len_b,
		float *max_coef,
		int *index, float *dif_coef,
		int *problem, int * n_index);
