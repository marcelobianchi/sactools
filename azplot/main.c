#include <stdio.h>
#include <stdlib.h>
#include <sac.h>
#include <head.h>
#include <ctl.h>

void d_angle(g_ctl *ctl, float *xdata, float *ydata, int npts) {
}

void d_time(g_ctl *ctl, float *data, SACHEAD *h) {
	int i;
	ctl_xreset_ndb(ctl, h->npts, h->delta, h->b);
	ctl_yupdate_ndb(ctl, data, h->npts, h->delta, h->b);
	ctl_resizeview(ctl);
	ctl_drawaxis(ctl);

	float *x = malloc(sizeof(float) * h->npts);
	for (i = 0; i < h->npts; i++)
		x[i] = i * h->delta + h->b;

	cpgline(h->npts, x, data);
	free(x);
}

void interact(float *z, SACHEAD *hz, float *n, SACHEAD *hn, float *e, SACHEAD *he) {
	int i;
	cpgopen("/Xwindow");
	g_ctl *zc, *nc, *ec, *azc, *inc;

	zc = ctl_newctl(0.05, 0.65, 0.50, 0.25);
	nc = ctl_newctl(0.05, 0.35, 0.50, 0.25);
	ec = ctl_newctl(0.05, 0.05, 0.50, 0.25);
	azc = ctl_newctl(0.60, 0.05, 0.35, 0.4);
	inc = ctl_newctl(0.60, 0.55, 0.35, 0.4);

	ctl_axisbottom(zc);
	ctl_axisbottom(nc);
	ctl_axisbottom(ec);
	ctl_axisbottom(azc);
	ctl_axisbottom(inc);

	d_time(zc, z, hz);
	d_time(nc, n, hn);
	d_time(ec, e, he);

	scanf("%d",&i);
}

int main(int argc, char **argv) {
	SACHEAD *hz, *hn, *he;
	float *z,*n,*e;
	int stop = 0;
	char *filename;

	filename = "z";
	z = io_readSac(filename, &hz);
	if (z == NULL) {
		fprintf(stderr, "Error reading z file: %s\n", filename);
		stop = 1;
	}

	filename = "n";
	n = io_readSac(filename, &hn);
	if (z == NULL) {
		fprintf(stderr, "Error reading n file: %s\n", filename);
		stop = 1;
	}

	filename = "e";
	e = io_readSac(filename, &he);
	if (z == NULL) {
		fprintf(stderr, "Error reading e file: %s\n", filename);
		stop = 1;
	}


	if (!stop) {
		interact(z,hz,n,hn,e,hn);
	}

	if (z) {
		free(z); z = NULL;
		free(hz); hz = NULL;
	}

	if (n) {
		free(n); n = NULL;
		free(hn); hn = NULL;
	}

	if (e) {
		free(e); e = NULL;
		free(he); he = NULL;
	}
}

