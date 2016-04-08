#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>
#include <math.h>

#include <sac.h>
#include <yutils.h>
#include <iir.h>

#include <head.h>
#include <ctl.h>
#include <interaction.h>

#define ZN 0
#define ZE 1
#define ZOOM 2
#define AZ 3
#define IN 4
#define OTHER 10

#define OFF 0
#define ON  1

float fh, fl; // Filter values
int filteris = OFF; // Filter status


void markazimuth(g_ctl *ctl, float mangle) {
    if (mangle > -999.0) {
        float xc = (ctl->xmin + ctl->xmax)/ 2.0;
        float yc = (ctl->xmin + ctl->xmax)/ 2.0;

        cpgsci(2);
        cpgslw(5);
        cpgmove(xc, yc);
        if (mangle < 180)
            cpgdraw(ctl->xmax, yc + (ctl->xmin - xc) * tan((mangle - 90) * M_PI / 180.0));
        else
            cpgdraw(ctl->xmin, -1 * (yc + (ctl->xmin - xc) * tan((mangle - 90) * M_PI / 180.0)));
        cpgsci(1);
        cpgslw(1);
    }
}

void markincidence(g_ctl *ctl, float mangle) {
    if (mangle > -999.0) {
        float xc = (ctl->xmin + ctl->xmax)/ 2.0;
        float yc = (ctl->xmin + ctl->xmax)/ 2.0;
        cpgsci(2);
        cpgslw(5);
        cpgmove(xc, yc);
        if (mangle > 0.)
            cpgdraw(ctl->xmax, (yc + (ctl->xmax - xc) * tan((mangle - 90) * M_PI / 180.0)));
        else
            cpgdraw(ctl->xmin, (yc + (ctl->xmin - xc) * tan((mangle - 90) * M_PI / 180.0)));

        cpgslw(1);
        cpgsci(1);
    }
}

void d_angle(g_ctl *ctl, float *xdata, float *ydata, int npts, float mangle) {
    float xmin, xmax;
    float ymin, ymax;
    char title[128] = "";

    if (mangle > -999.0) {
        sprintf(title, "Angle is %.1f", mangle);
        strcpy(ctl->title, title);
    } else {
        strcpy(ctl->title, "");
    }

    vecminmax(xdata, npts, &xmin, &xmax, NULL, NULL);
    vecminmax(ydata, npts, &ymin, &ymax, NULL, NULL);

    if (xmin > ymin) xmin = ymin;
    if (xmax < ymax) xmax = ymax;

    if (fabs(xmin) > fabs(xmax)) {
        ctl_xreset_mm(ctl, xmin, -1 * xmin);
        ctl_yreset_mm(ctl, xmin, -1 * xmin);
    } else {
        ctl_xreset_mm(ctl, -1 * xmax, xmax);
        ctl_yreset_mm(ctl, -1 * xmax, xmax);
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

	if (filteris == ON) {
		char texto[256];
		cpgsch(0.7);
		cpgsci(3);
		sprintf(texto, "Filter On [%.2f/%.2f]",fh,fl);
		cpgmtxt("L", 0.4, 0.5, 0.5, texto);
		cpgsch(1.0);
		cpgsci(1);
	}

	free(x);
}

void line(g_ctl *ctl, float xmin, int color) {
    cpgsci(color);
    cpgmove(xmin, ctl->ymin + ctl->h * 0.2);
    cpgdraw(xmin, ctl->ymax - ctl->h * 0.2);
    cpgsci(1);
}

char *get(char *input) {
    if (strncmp(input, "-12345", 6) == 0) return "- n/a -";
    return input;
}

char *getf(float v, char *fmt) {
    static char temp[256];
    if (v == -12345.0) return " - ";
    sprintf(temp, fmt, v);
    return temp;
}

void tag(SACHEAD *h) {
    char texto[256] = "";

    g_ctl *zc = ctl_newctl(0.05, 0.05,0.90,0.90);
    ctl_resizeview(zc);

    sprintf(texto,
            "%s.%s: %04d-%03d/%02d:%02d:%02d (Event Id: %s)",
            get(h->knetwk), get(h->kstnm), h->nzyear, h->nzjday, h->nzhour, h->nzmin, h->nzsec, get(h->kevnm));
    sprintf(texto, "%s Lon: %s%c", texto, getf(h->evlo, "%7.2f"), (char)94);
    sprintf(texto, "%s Lat: %s%c", texto, getf(h->evla, "%6.2f"), (char)94);
    sprintf(texto, "%s Dep: %s", texto, getf((h->evdp>1000.0)?h->evdp / 1000.0:h->evdp, "%5.1f km"));
    sprintf(texto, "%s Mag: %s", texto, getf(h->mag, "%3.1f"));
    cpgmtxt("T", 1.2, 0.5, 0.5, texto);

    sprintf(texto, "Az: %s%c", getf(h->az, "%5.1f"), (char)94);
    sprintf(texto, "%s Baz: %s%c", texto, getf(h->baz, "%5.1f"), (char)94);
    sprintf(texto, "%s Distance: %s", texto, getf(h->dist, "%5.1f km"));
    sprintf(texto, "%s / %s%c", texto, getf(h->gcarc, "%5.1f"), (char)94);
    cpgmtxt("T", 0.0, 0.5, 0.5, texto);

    cpgmtxt("R", 2.0, 0.0, 0.0, "AzPlot / m.bianchi@iag.usp.br @ 2016");

    free(zc);
}

void d(g_ctl *zc, g_ctl *nc, g_ctl *ec, g_ctl *azc, g_ctl *inc,
       float *z, SACHEAD *hz, float *n, SACHEAD *hn, float *e, SACHEAD *he,
       float xmin, float xmax, float azimuth, float incidence, int inct) {
    cpgeras();

    /* Ts + times */
    d_time(zc, z, hz);
    if (xmin < xmax) {
        line(zc, xmin, 3);
        line(zc, xmax, 3);
    }

    d_time(nc, n, hn);
    if (xmin < xmax) {
        line(zc, xmin, 3);
        line(zc, xmax, 3);
    }

    d_time(ec, e, he);
    if (xmin < xmax) {
        line(zc, xmin, 3);
        line(zc, xmax, 3);
    }

    /* Az plots */
    if (xmin < xmax) {
        int np = (int)((xmax - xmin) / hz->delta + 0.5) + 1;
        int i = (int)((xmin - hz->b) / hz->delta + 0.5) + 1;

        if (i < 0) i = 0;
        if (i + np > hz->npts) np = hz->npts - i;

        d_angle(azc, &e[i], &n[i], np, azimuth);
        markazimuth(azc, azimuth);

        if (inct == ZE) {
            strcpy(inc->xlabel, "EW");
            d_angle(inc, &e[i], &z[i], np, incidence);
        } else {
            strcpy(inc->xlabel, "NS");
            d_angle(inc, &n[i], &z[i], np, incidence);
        }
        markincidence(inc, incidence);
    }
    tag(hz);
    return;
}

void interact(float *z, SACHEAD *hz, float *n, SACHEAD *hn, float *e, SACHEAD *he) {
    int i;

	float *zf = z;
	float *nf = n;
	float *ef = e;

    i = opengr();
    resizemax(0.8);

    g_ctl *zc, *nc, *ec, *azc, *inc, *hitc;

    zc = ctl_newctl(0.35, 0.63, 0.6, 0.25);
    nc = ctl_newctl(0.35, 0.35, 0.6, 0.25);
    ec = ctl_newctl(0.35, 0.07, 0.6, 0.25);

    azc = ctl_newctl(0.05, 0.52, 0.23, 0.23 / BASIC_ASPECT);
    inc = ctl_newctl(0.05, 0.07, 0.23, 0.23 / BASIC_ASPECT);

    ctl_xreset_ndb(zc, hz->npts, hz->delta, hz->b);
    ctl_xreset_ndb(nc, hn->npts, hn->delta, hn->b);
    ctl_xreset_ndb(ec, he->npts, he->delta, he->b);

    ctl_axisbottom(zc);
    ctl_axisbottom(nc);
    ctl_axisbottom(ec);

    ctl_axismap(azc);
    ctl_axismap(inc);

    strncpy(azc->xaxis, "BCST", 14);
    strncpy(azc->yaxis, "BCST", 14);

    strncpy(inc->xaxis, "BCST", 14);
    strncpy(inc->yaxis, "BCST", 14);

    strcpy(ec->xlabel, "Time (s)");

    strcpy(zc->ylabel, "Z");
    strcpy(nc->ylabel, "N");
    strcpy(ec->ylabel, "E");

    strcpy(azc->xlabel, "EW");
    strcpy(azc->ylabel, "NS");
    strcpy(inc->ylabel, "Z");

    azc->xlabel_offset = 0.5;
    inc->xlabel_offset = 0.5;
    azc->ylabel_offset = 0.5;
    inc->ylabel_offset = 0.5;

    /* Interaction */
    float ax, ay;
    float a1, a2;
    char ch = ' ';

    /* Time Window */
    float t1, t2;

    /* Angles */
    float azimuth, incidence;

    int inct = ZE;
    int mode = 0;

    g_ctl *ctlpick = NULL;
    ctlpick = ctl_newctl(0.0, 0.0, 1.0, 1.0);
    ctlpick->expand = 0;

    t1 = t2 = 0.0;
    azimuth   = -999.0;
    incidence = -999.0;

    while (ch != 'Q') {
                d(zc, nc, ec, azc, inc, zf, hz, nf, hn, ef, he, t1, t2, azimuth, incidence, inct);
        {
            hitc = NULL;
            ctl_resizeview(ctlpick);
            float x, y;
            ch = toupper(getonechar(&ax, &ay));
            if (ctl_checkhit(zc, ax, ay)) {
                hitc = zc;
                mode = ZOOM;
            } else if (ctl_checkhit(nc, ax, ay)) {
                hitc = nc;
                mode = ZOOM;
            } else if (ctl_checkhit(ec, ax, ay)) {
                hitc = ec;
                mode = ZOOM;
            } else if (ctl_checkhit(azc, ax, ay)) {
                hitc = azc;
                mode = AZ;
            } else if (ctl_checkhit(inc, ax, ay)) {
                hitc = inc;
                mode = IN;
            } else {
                hitc = ctlpick;
                mode = OTHER;
            }
            if (hitc == NULL) continue;
            ctl_convertxy(hitc, ax, ay, &x, &y);
            ctl_resizeview(hitc);
            ax = x; ay = y;
        }
        switch (ch) {

        case 'P':
            cpgclos();
            cpgopen("azplot.ps/CPS");
                        d(zc, nc, ec, azc, inc, zf, hz, nf, hn, ef, he, t1, t2, azimuth, incidence, inct);
            cpgclos();
            i = opengr();
            resizemax(0.8);
                        d(zc, nc, ec, azc, inc, zf, hz, nf, hn, ef, he, t1, t2, azimuth, incidence, inct);
            sprintf(message, "Print wrote to: azplot.ps");
            alert(ANOUNCE);
            break;

        case 'T':
            inct = (inct == ZE) ? ZN : ZE;
            incidence = -999.0;
            break;

        case 'X':
            if (mode != ZOOM) break;
            a1 = ax;
            line(hitc, a1, 2);
            ch = toupper(getonechar(&ax, &ay));
            a2 = ax;
            if (a1 >= a2) {
                ctl_xreset_ndb(zc, hz->npts, hz->delta, hz->b);
                ctl_xreset_ndb(nc, hn->npts, hn->delta, hn->b);
                ctl_xreset_ndb(ec, he->npts, he->delta, he->b);
            } else {
                ctl_xreset_mm(zc, a1, a2);
                ctl_xreset_mm(nc, a1, a2);
                ctl_xreset_mm(ec, a1, a2);
            }
            break;

        case 'A':
            if (mode == ZOOM) {
                azimuth = -999.0;
                incidence = -999.0;
                t1 = ax;
                line(hitc, t1, 3);
                ch = toupper(getonechar(&ax, &ay));
                t2 = ax;
                if (t1 > t2) {
                    ax = t1;
                    t1 = t2;
                    t2 = ax;
                }
            } else if (mode == AZ) {
                cpgsci(2);
                a1 = (hitc->xmax + hitc->xmin) / 2.0;
                a2 = (hitc->ymax + hitc->ymin) / 2.0;
                cpgband(1, 0, a1, a2, &ax, &ay, &ch);
                cpgsci(1);

                azimuth = (atan2f(ax-a1, ay-a2) * 180.0 / M_PI);
                if (azimuth < 0) azimuth += 360.0;
            } else if (mode == IN) {
                cpgsci(2);
                a1 = (hitc->xmax + hitc->xmin) / 2.0;
                a2 = (hitc->ymax + hitc->ymin) / 2.0;
                cpgband(1, 0, a1, a2, &ax, &ay, &ch);
                cpgsci(1);

                incidence = (atan2f(ax-a1, a2- ay) * 180.0 / M_PI);
            }
            break;

		case 'F':
			if (filteris == OFF) {
				fh = lerfloat("High pass frequency (0 to cancel)?");
				if (fh == 0.0) break;
				fl = lerfloat("Low Pass frequency (0 to cancel)?");
				if (fl == 0.0) break;
				if (fh >= fl) break;

				zf = iir(z, hz->npts, hz->delta, 2.0, fh, 2.0, fl);
				nf = iir(n, hn->npts, hn->delta, 2.0, fh, 2.0, fl);
				ef = iir(e, he->npts, he->delta, 2.0, fh, 2.0, fl);

				filteris = ON;
			} else {
				free(zf);
				zf = z;
				free(nf);
				nf = n;
				free(ef);
				ef = e;
				filteris = OFF;
				fh = 0.0;
				fl = 0.0;
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

    hz = hn = he = NULL;
    z = n = e = NULL;
    filename = NULL;

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

    /* check that files reference and sps match ! */
    if (!stop) {
        if (hz->nzyear != he->nzyear ||
                hz->nzjday != he->nzjday ||
                hz->nzmin != he->nzmin ||
                hz->nzhour != he->nzhour ||
                hz->nzsec != he->nzsec ||
                hz->nzmsec != he->nzmsec ||
                hz->b != he->b ||
                hz->delta != he->delta ||
                hz->npts != he->npts) {
            stop = 1;
            fprintf(stderr, "Z and E files reference time differ.\n");
        }

        if (hz->nzyear != hn->nzyear ||
                hz->nzjday != hn->nzjday ||
                hz->nzmin != hn->nzmin ||
                hz->nzhour != hn->nzhour ||
                hz->nzsec != hn->nzsec ||
                hz->nzmsec != hn->nzmsec ||
                hz->b != hn->b ||
                hz->delta != hn->delta ||
                hz->npts != hn->npts) {
            stop = 1;
            fprintf(stderr, "Z and N files reference time differ.\n");
        }
    }

    if (!stop) {
                yu_rtrend(z, hz->npts);
                yu_rtrend(n, hn->npts);
                yu_rtrend(e, he->npts);

        if (z == NULL)fprintf(stderr, "OILA");
        interact(z,hz,n,hn,e,hn);
    }

    if (z != NULL) {
        free(z); z = NULL;
        free(hz); hz = NULL;
    }

    if (n != NULL) {
        free(n); n = NULL;
        free(hn); hn = NULL;
    }

    if (e != NULL) {
        free(e); e = NULL;
        free(he); he = NULL;
    }
}

/*
 * To prepare file one can use SAC
 *
 * cut a -100 600; r
 * bp cor 0.7 2.0 p 2 n 3
 * sync
 * w z n e
 */
