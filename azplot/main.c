#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>

#include <sac.h>
#include <yutils.h>

#include <head.h>
#include <ctl.h>
#include <interaction.h>

void d_angle(g_ctl *ctl, float *xdata, float *ydata, int npts) {
    float xmin, xmax;

    vecminmax(xdata, npts, &xmin, &xmax, NULL, NULL);
    if (fabs(xmin) > fabs(xmax)) {
        ctl->xmin = xmin;
        ctl->xmax = -1 * xmin;
    } else {
        ctl->xmin = -1 * xmax;
        ctl->xmax = xmax;
    }

    vecminmax(ydata, npts, &xmin, &xmax, NULL, NULL);
    if (fabs(xmin) > fabs(xmax)) {
        ctl->ymin = xmin;
        ctl->ymax = -1 * xmin;
    } else {
        ctl->ymin = -1 * xmax;
        ctl->ymax = xmax;
    }

    ctl_resizeview(ctl);
    ctl_drawaxis(ctl);

    cpgline(npts, xdata, ydata);
}

void d_time(g_ctl *ctl, float *data, SACHEAD *h) {
	int i;

	ctl_yupdate_ndb(ctl, data, h->npts, h->delta, h->b);
	ctl_resizeview(ctl);
	ctl_drawaxis(ctl);

	float *x = malloc(sizeof(float) * h->npts);
	for (i = 0; i < h->npts; i++)
		x[i] = i * h->delta + h->b;

	cpgline(h->npts, x, data);
	free(x);
}

void line(g_ctl *ctl, float xmin) {
    cpgmove(xmin, ctl->ymin);
    cpgdraw(xmin, ctl->ymax);
}

void d(g_ctl *zc, g_ctl *nc, g_ctl *ec, g_ctl *azc, g_ctl *inc, float *z, SACHEAD *hz, float *n, SACHEAD *hn, float *e, SACHEAD *he, float xmin, float xmax) {
    cpgeras();

    /* Ts + times */
    d_time(zc, z, hz);
    if (xmin < xmax) {
        line(zc, xmin);
        line(zc, xmax);
    }

    d_time(nc, n, hn);
    if (xmin < xmax) {
        line(zc, xmin);
        line(zc, xmax);
    }

    d_time(ec, e, he);
    if (xmin < xmax) {
        line(zc, xmin);
        line(zc, xmax);
    }

    /* Az plots */
    if (xmin < xmax) {
        int np = (int)((xmax - xmin) / hz->delta + 0.5) + 1;
        int i = (int)((xmin - hz->b) / hz->delta + 0.5) + 1;

        if (i < 0) i = 0;
        if (i + np > hz->npts) np = hz->npts - i;

        fprintf(stderr,"%f %f %f N => %d I => %d\n", xmin, xmax, he->delta, np, i);

        d_angle(azc, &n[i], &e[i], np);
        d_angle(inc, &z[i], &e[i], np);
    }
    return;
}

void interact(float *z, SACHEAD *hz, float *n, SACHEAD *hn, float *e, SACHEAD *he) {
	int i;

    i = opengr();
    resizemax(0.8);

    g_ctl *zc, *nc, *ec, *azc, *inc;

    zc = ctl_newctl(0.05, 0.70, 0.50, 0.25);
    nc = ctl_newctl(0.05, 0.40, 0.50, 0.25);
    ec = ctl_newctl(0.05, 0.1, 0.50, 0.25);

    azc = ctl_newctl(0.60, 0.55, 0.35, 0.3);
    inc = ctl_newctl(0.60, 0.1, 0.35, 0.3);

    ctl_xreset_ndb(zc, hz->npts, hz->delta, hz->b);
    ctl_xreset_ndb(nc, hn->npts, hn->delta, hn->b);
    ctl_xreset_ndb(ec, he->npts, he->delta, he->b);

	ctl_axisbottom(zc);
	ctl_axisbottom(nc);
	ctl_axisbottom(ec);
    ctl_axismap(azc);
    ctl_axismap(inc);

    strcpy(zc->xlabel, "Time (s)");
    strcpy(nc->xlabel, "Time (s)");
    strcpy(ec->xlabel, "Time (s)");
    strcpy(zc->ylabel, "Z");
    strcpy(nc->ylabel, "N");
    strcpy(ec->ylabel, "E");

    float ax = 0.0, ay = 0.0;
    char ch = ' ';
    float xmin, xmax;
    xmin = xmax = 0.0;

    float a1, a2;
    while (ch != 'Q') {
        d(zc, nc, ec, azc, inc, z, hz, n, hn, e, he, xmin, xmax);
        ctl_resizeview(zc);
        ch = toupper(getonechar(&ax, &ay));
        switch (ch) {
        case 'X':
            a1 = ax;
            ch = toupper(getonechar(&ax, &ay));
            a2 = ax;
            if (a1 >= a2) {
                ctl_xreset_ndb(zc, hz->npts, hz->delta, hz->b);
                ctl_xreset_ndb(nc, hn->npts, hn->delta, hn->b);
                ctl_xreset_ndb(ec, he->npts, he->delta, he->b);
            } else {
                zc->xmin = ec->xmin = nc->xmin = a1;
                zc->xmax = ec->xmax = nc->xmax = a2;
            }
            break;

        case 'A':
            xmin = ax;
            ch = toupper(getonechar(&ax, &ay));
            xmax = ax;
            if (xmin > xmax) {
                ax = xmin;
                xmin = xmax;
                xmax = ax;
            }
            break;

        case 'Q':
            break;

        default:
            printf("Oops, invalid command.");
            break;
        }
    }
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

