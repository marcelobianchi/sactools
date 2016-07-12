#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
 * Adapted from Correlation and interpolation methods from Caio Ciardelli correlation program.
 */
 
void parabola(float *lags, int n_lags, int index, float best_lag, float dt, float *xmax, float *ymax) {
    float xbefore, x, xafter;
    float ybefore, y, yafter;
    float a, b, c;
    
    xbefore = best_lag - dt;
    x       = best_lag;
    xafter  = best_lag + dt;
    
    ybefore = lags[index - 1];
    y       = lags[index];
    yafter  = lags[index + 1];
    
    a = ((yafter - y) / dt - (y - ybefore) / dt) / (2 * dt);
    b = ((y-ybefore)/dt) - a*(x-xbefore);
    c = ybefore + (a*x-((y-ybefore)/dt)) * xbefore;

    *xmax = -b / (2*a);
    *ymax = (4*a*c - b*b ) / (4*a);
    
    return;
}

float *correl (float *tra, float *trb, int len_a, int len_b, float *max_coef, int *index, float *dif_coef, int *problem, int * n_index)
{
    int i, j;
    int len_trc = len_a;
    float s, n1, fir_max = 0, sec_max = 0, *lags = NULL;

    // Initialization
    fir_max = 0.;
    sec_max = 0.;
    s = 0.0;

    *max_coef  = 0.0;
    *index     = -1;
    *dif_coef = 0.0;
    *problem   = 0;

    *n_index = (len_b - len_a + 1);
    lags = malloc ((*n_index) * sizeof (float));

    // tra Main Normalization
    for (j = 0; j < len_a; j++)
        s += tra[j] * tra[j];
    n1 = sqrt(s);

    // Main Loop
    for (i = 0; i < (*n_index); i++) {
        int j;
        double coef = 0.0;

        s = 0.0;
        for (j = 0; j < len_trc; j++) {
            s += trb[i + j] * trb[i + j];
            coef += tra[j] * trb[i + j];
        }

        double n2 = sqrt (s);
        coef /= n1 * n2;
        lags[i] = coef;

        if (coef > *max_coef) {
            *max_coef = coef;
        }

        if (i > 1 && lags[i - 1] > lags[i - 2] && lags[i - 1] > lags[i]) {
            if (lags[i - 1] > fir_max) {
                sec_max = fir_max;
                fir_max = lags[i - 1];
                *index = i - 1;
            } else if (lags[i - 1] > sec_max) {
                sec_max = lags[i - 1];
            }
        }
    }

    if (*max_coef == fir_max) {
        *problem = 1;
    } else {
        *max_coef = fir_max;
        *problem = 1;
    }

    *dif_coef = fir_max - sec_max;

    return lags;
}

/*
int main (int argc, char ** argv) {
	int i;
	double *a;
	double *b;
	int na = 5;
	int nb = 20;

	a = malloc(sizeof(double) * na);
	b = malloc(sizeof(double) * nb);

	for(i=0;i<na;i++)
		a[i] = 1;

	for(i=0;i<nb;i++)
		b[i] = 1;

	a[2] = 2;
	b[10] = 2;

	double max_coef, dif_coef;
	bool problem;
	int index, nlags;

	double *lags = correl(a, b, na, nb, &max_coef, &index, &dif_coef, &problem, &nlags);
	fprintf(stderr, "Max: %lg Dif: %lg Index: %d Prob.: %d nLags: %d\n",max_coef, dif_coef, index, problem, nlags);

	for(i = 0; i < nlags; i ++)
		fprintf(stderr," %03d: %lg\n", i, lags[i]);
}
*/
