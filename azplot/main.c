#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>
#include <math.h>
#include <strings.h>
#include <sac.h>
#include <yutils.h>
#include <iir.h>

#include <head.h>
#include <ctl.h>
#include <interaction.h>
#include <timeu.h>

#include "model.h"

#define ZN 0
#define ZE 1
#define ZOOM 2
#define AZ 3
#define IN 4
#define OTHER 10

#define OFF 0
#define ON  1

float fh, fl;			// Filter values
int filteris = OFF;		// Filter status
char statusline[256];	// Message to display on status

int rotated = OFF;		// Is rotated trace?

#define nm 12
typedef struct m {
	int set;
	float time;
	char name[9];
} M;

M marks[nm];

void rotateNE2RT(int n, float *E, float *N, float azimuth)
{
	int i;
	float ntmp, etmp;

	for (i = 0; i < n; i++) {
		etmp = E[i];
		ntmp = N[i];

		E[i] = etmp * cos(azimuth) - ntmp * sin(azimuth);
		N[i] = etmp * sin(azimuth) + ntmp * cos(azimuth);
	}
}

void line(g_ctl * ctl, float xmin, int color)
{
	if (xmin > ctl->xmax || xmin < ctl->xmin)
		return;
	cpgsci(color);
	cpgmove(xmin, ctl->ymin + ctl->h * 0.2);
	cpgdraw(xmin, ctl->ymax - ctl->h * 0.2);
	cpgsci(1);
}

void markazimuth(g_ctl * ctl, float mangle)
{
	if (mangle > SAC_HEADER_FLOAT_UNDEFINED) {
		float xc = (ctl->xmin + ctl->xmax) / 2.0;
		float yc = (ctl->xmin + ctl->xmax) / 2.0;

		cpgsci(2);
		cpgslw(5);
		cpgmove(xc, yc);
		if (mangle < 180)
			cpgdraw(ctl->xmax,
				yc + (ctl->xmin -
				      xc) * tan((mangle - 90) * M_PI / 180.0));
		else
			cpgdraw(ctl->xmin,
				-1 * (yc +
				      (ctl->xmin -
				       xc) * tan((mangle -
						  90) * M_PI / 180.0)));
		cpgsci(1);
		cpgslw(1);
	}
}

void markincidence(g_ctl * ctl, float mangle)
{
	if (mangle > SAC_HEADER_FLOAT_UNDEFINED) {
		float xc = (ctl->xmin + ctl->xmax) / 2.0;
		float yc = (ctl->xmin + ctl->xmax) / 2.0;
		cpgsci(2);
		cpgslw(5);
		cpgmove(xc, yc);
		if (mangle > 0.)
			cpgdraw(ctl->xmax,
				(yc +
				 (ctl->xmax -
				  xc) * tan((mangle - 90) * M_PI / 180.0)));
		else
			cpgdraw(ctl->xmin,
				(yc +
				 (ctl->xmin -
				  xc) * tan((mangle - 90) * M_PI / 180.0)));

		cpgslw(1);
		cpgsci(1);
	}
}

void d_angle(g_ctl * ctl, float *xdata, float *ydata, int npts, float mangle)
{
	float xmin, xmax;
	float ymin, ymax;
	char title[128] = "";

	if (mangle > SAC_HEADER_FLOAT_UNDEFINED) {
		sprintf(title, "Angle is %.1f", mangle);
		strcpy(ctl->title, title);
	} else {
		strcpy(ctl->title, "");
	}

	vecminmax(xdata, npts, &xmin, &xmax, NULL, NULL);
	vecminmax(ydata, npts, &ymin, &ymax, NULL, NULL);

	if (xmin > ymin)
		xmin = ymin;
	if (xmax < ymax)
		xmax = ymax;

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

void d_time(g_ctl * ctl, float *data, SACHEAD * h)
{
	int i;

	ctl_yupdate_ndb(ctl, data, h->npts, h->delta, h->b);
	ctl_resizeview(ctl);
	ctl_drawaxis(ctl);

	float *x = malloc(sizeof(float) * h->npts);
	for (i = 0; i < h->npts; i++)
		x[i] = i * h->delta + h->b;

	cpgline(h->npts, x, data);

	if (filteris == ON) {
		char texto[1024];
		cpgsch(0.7);
		cpgsci(3);
		sprintf(texto, "Filter On [%.2f/%.2f] %s", fh, fl,
			(rotated == ON) ? "R" : "-");
		cpgmtxt("L", 0.4, 0.5, 0.5, texto);
		cpgsch(1.0);
		cpgsci(1);
	} else if (rotated == ON) {
		char texto[1024];
		cpgsch(0.7);
		cpgsci(3);
		sprintf(texto, " - No Filter - R");
		cpgmtxt("L", 0.4, 0.5, 0.5, texto);
		cpgsch(1.0);
		cpgsci(1);
	}

	for (i = 0; i < nm; i++) {
		if (marks[i].set) {
			char l[128];

			line(ctl, marks[i].time, 5);
			if (marks[i].time > ctl->xmin
			    && marks[i].time < ctl->xmax) {
				sprintf(l, "%s [%1d]", marks[i].name, i);
				cpgsch(0.6);
				cpgtext(marks[i].time, ctl->ymin + ctl->h * 0.2,
					l);
				cpgsch(1.0);
			}
		}
	}

	free(x);
}

char *getf(float v, char *fmt)
{
	static char temp[256];
	if (v == -12345.0)
		return " - ";
	sprintf(temp, fmt, v);
	return temp;
}

void tag(SACHEAD * h, int needsave)
{
	char texto[2048];

	g_ctl *zc = ctl_newctl(0.05, 0.05, 0.90, 0.90);
	ctl_resizeview(zc);

	char *net = hd_showValueFromChar(h, "knetwk", NULL, NULL, NULL);
	char *sta = hd_showValueFromChar(h, "kstnm", NULL, NULL, NULL);
	char *ev = hd_showValueFromChar(h, "kevnm", NULL, NULL, NULL);
	if (strncmp(sta, "-12345", 6) == 0) strcpy(sta, " - ");
	if (strncmp(net, "-12345", 6) == 0) strcpy(net, " - ");
	if (strncmp(ev,"-12345", 6) == 0)
		strcpy(ev, " - ");
	sprintf(texto,
		"%s.%s: %04d-%03d/%02d:%02d:%02d (Event Id: %s)",
		net, sta, h->nzyear, h->nzjday, h->nzhour, h->nzmin, h->nzsec,
		ev);
	free(net);
	free(sta);
	free(ev);

	sprintf(texto, "%s Lon: %s%c", texto, getf(h->evlo, "%7.2f"), (char)94);
	sprintf(texto, "%s Lat: %s%c", texto, getf(h->evla, "%6.2f"), (char)94);
	sprintf(texto, "%s Dep: %s", texto,
		getf((h->evdp > 1000.0) ? h->evdp / 1000.0 : h->evdp,
		     "%5.1f km"));
	sprintf(texto, "%s Mag: %s", texto, getf(h->mag, "%3.1f"));
	cpgmtxt("T", 1.2, 0.5, 0.5, texto);

	if (needsave) {
		cpgsci(2);
		sprintf(texto, "File is Modified");
		cpgmtxt("T", -1.2, 0.5, 0.5, texto);
		cpgsci(1);
	}
	
	sprintf(texto, "Az: %s%c", getf(h->az, "%5.1f"), (char)94);
	sprintf(texto, "%s Baz: %s%c", texto, getf(h->baz, "%5.1f"), (char)94);
	sprintf(texto, "%s Distance: %s", texto, getf(h->dist, "%5.1f km"));
	sprintf(texto, "%s / %s%c", texto, getf(h->gcarc, "%5.1f"), (char)94);
	cpgmtxt("T", 0.0, 0.5, 0.5, texto);

	if (strlen(statusline) != 0) {
		cpgmtxt("T", -1.2, 0.5, 0.5, statusline);
		strcpy(statusline, "");
	}
	cpgmtxt("R", 2.0, 0.0, 0.0, "AzPlot / m.bianchi@iag.usp.br @ 2016");

	free(zc);
}

void
d(g_ctl * zc, g_ctl * nc, g_ctl * ec, g_ctl * azc, g_ctl * inc,
  float *z, SACHEAD * hz, float *n, SACHEAD * hn, float *e, SACHEAD * he,
  float xmin, float xmax, float azimuth, float incidence, int inct, int needsave)
{
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

		if (i < 0)
			i = 0;
		if (i + np > hz->npts)
			np = hz->npts - i;

		d_angle(azc, &e[i], &n[i], np, azimuth);
		markazimuth(azc, azimuth);

		if (inct == ZE) {
			strcpy(inc->xlabel, (rotated == ON) ? "T" : "WE");
			d_angle(inc, &e[i], &z[i], np, incidence);
		} else {
			strcpy(inc->xlabel, (rotated == ON) ? "R" : "SN");
			d_angle(inc, &n[i], &z[i], np, incidence);
		}
		markincidence(inc, incidence);
	}

	tag(hz, needsave);

	return;
}

void savemark(M *m, SACHEAD *hz, SACHEAD *hn, SACHEAD *he, char *header, char *lheader) {
	if (m->set) {
		hdu_changeValueFromChar(hz, header, &m->time, NULL, NULL);
		hdu_changeValueFromChar(hn, header, &m->time, NULL, NULL);
		hdu_changeValueFromChar(he, header, &m->time, NULL, NULL);

		hdu_changeValueFromChar(hz, lheader, NULL, NULL, &m->name);
		hdu_changeValueFromChar(hn, lheader, NULL, NULL, &m->name);
		hdu_changeValueFromChar(he, lheader, NULL, NULL, &m->name);
	} else {
		float v = SAC_HEADER_FLOAT_UNDEFINED;
		hdu_changeValueFromChar(hz, header, &v, NULL, NULL);
		hdu_changeValueFromChar(hn, header, &v, NULL, NULL);
		hdu_changeValueFromChar(he, header, &v, NULL, NULL);

		hdu_changeValueFromChar(hz, lheader, NULL, NULL, SAC_HEADER_CHAR_UNDEFINED);
		hdu_changeValueFromChar(hn, lheader, NULL, NULL, SAC_HEADER_CHAR_UNDEFINED);
		hdu_changeValueFromChar(he, lheader, NULL, NULL, SAC_HEADER_CHAR_UNDEFINED);
	}
}

void
loadmark(M *m, SACHEAD *h, char *header, char *lheader) {
	char *name = NULL;
	
	m->set = 1;
	
	hdu_getValueFromChar(header, h, &m->time, NULL, NULL);
	hdu_getValueFromChar(lheader, h, NULL, NULL, &name);
	strcpy(m->name, name);

	if (name != NULL) free(name);

	return;
}

void
interact(char *zfilename, float *z, SACHEAD * hz, char *nfilename, float *n, SACHEAD * hn, char *efilename, float *e, SACHEAD * he)
{
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

	strcpy(azc->xlabel, "WE");
	strcpy(azc->ylabel, "SN");
	strcpy(inc->ylabel, "Z");

	azc->xlabel_offset = 0.5;
	inc->xlabel_offset = 0.5;
	azc->ylabel_offset = 0.5;
	inc->ylabel_offset = 0.5;

	for (i = 0; i < nm; i++) {
		marks[i].set = 0;
	}

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

	int needsave = 0;

	g_ctl *ctlpick = NULL;
	ctlpick = ctl_newctl(0.0, 0.0, 1.0, 1.0);
	ctlpick->expand = 0;

	/*
	 *  Loading parameters from hz fileheader
	 */
	incidence = (hz->unused7 != SAC_HEADER_FLOAT_UNDEFINED) ? hz->unused7 : SAC_HEADER_FLOAT_UNDEFINED;
	azimuth   = (hz->unused6 != SAC_HEADER_FLOAT_UNDEFINED) ? hz->unused6 : SAC_HEADER_FLOAT_UNDEFINED;
	t1   = (hz->unused8 != SAC_HEADER_FLOAT_UNDEFINED) ? hz->unused8 : 0.0;
	t2   = (hz->unused9 != SAC_HEADER_FLOAT_UNDEFINED) ? hz->unused9 : 0.0;
	fh   = (hz->unused10 != SAC_HEADER_FLOAT_UNDEFINED) ? hz->unused10 : 5.;
	fl   = (hz->unused11 != SAC_HEADER_FLOAT_UNDEFINED) ? hz->unused11 : 25.;
	inct = (hz->unused15 != SAC_HEADER_INT_UNDEFINED) ? hz->unused15 : ZE;
	
	if ( (fh != SAC_HEADER_FLOAT_UNDEFINED) && (SAC_HEADER_FLOAT_UNDEFINED != fl)) {
		zf = iir(z, hz->npts, hz->delta, 2.0, fh, 2.0,
			 fl);
		nf = iir(n, hn->npts, hn->delta, 2.0, fh, 2.0,
			 fl);
		ef = iir(e, he->npts, he->delta, 2.0, fh, 2.0,
			 fl);
		filteris = ON;
	}
	
	i = 0;
	if (hz->a != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "a", "ka");
	if (hz->t0 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t0", "kt0");
	if (hz->t1 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t1", "kt1");
	if (hz->t2 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t2", "kt2");
	if (hz->t3 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t3", "kt3");
	if (hz->t4 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t4", "kt4");
	if (hz->t5 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t5", "kt5");
	if (hz->t6 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t6", "kt6");
	if (hz->t7 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t7", "kt7");
	if (hz->t8 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t8", "kt8");
	if (hz->t9 != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "t9", "kt9");
	if (hz->f != SAC_HEADER_FLOAT_UNDEFINED) loadmark(&marks[i++], hz, "f", "kf");

	float *hitdata;
	SACHEAD *hithead;
	while (ch != 'Q') {
		d(zc, nc, ec, azc, inc, zf, hz, nf, hn, ef, he, t1, t2, azimuth,
		  incidence, inct, needsave);
		{
			hitc = NULL;
			ctl_resizeview(ctlpick);

			float x, y;
			ch = toupper(getonechar(&ax, &ay));

			if (ctl_checkhit(zc, ax, ay)) {
				hitc = zc;
				mode = ZOOM;
				hitdata = zf;
				hithead = hz;
			} else if (ctl_checkhit(nc, ax, ay)) {
				hitc = nc;
				mode = ZOOM;
				hitdata = nf;
				hithead = hn;
			} else if (ctl_checkhit(ec, ax, ay)) {
				hitc = ec;
				mode = ZOOM;
				hitdata = ef;
				hithead = he;
			} else if (ctl_checkhit(azc, ax, ay)) {
				hitc = azc;
				mode = AZ;
				hitdata = NULL;
				hithead = NULL;
			} else if (ctl_checkhit(inc, ax, ay)) {
				hitc = inc;
				mode = IN;
				hitdata = NULL;
				hithead = NULL;
			} else {
				hitc = ctlpick;
				mode = OTHER;
				hitdata = NULL;
				hithead = NULL;
			}
			if (hitc == NULL)
				continue;
			ctl_convertxy(hitc, ax, ay, &x, &y);
			ctl_resizeview(hitc);
			ax = x;
			ay = y;
		}
		switch (ch) {

		case 'T':	// Switch Vaz mode.
			inct = (inct == ZE) ? ZN : ZE;
			incidence = SAC_HEADER_FLOAT_UNDEFINED;
			needsave = 1;
			break;

		case 'X':	// Zoom trace
			if (mode != ZOOM)
				break;
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

		case 'A':	// Define a azimuth window
			if (mode == ZOOM) {
				azimuth = SAC_HEADER_FLOAT_UNDEFINED;
				incidence = SAC_HEADER_FLOAT_UNDEFINED;
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

				azimuth =
				    (atan2f(ax - a1, ay - a2) * 180.0 / M_PI);
				if (azimuth < 0)
					azimuth += 360.0;
			} else if (mode == IN) {
				cpgsci(2);
				a1 = (hitc->xmax + hitc->xmin) / 2.0;
				a2 = (hitc->ymax + hitc->ymin) / 2.0;
				cpgband(1, 0, a1, a2, &ax, &ay, &ch);
				cpgsci(1);

				incidence =
				    (atan2f(ax - a1, a2 - ay) * 180.0 / M_PI);
			}
			needsave = 1;
			break;

		case 'F':	// Filter on/off
			if (filteris == OFF) {
				fh = lerfloat
				    ("High pass frequency (0 to cancel)?");
				if (fh == 0.0)
					break;
				fl = lerfloat
				    ("Low Pass frequency (0 to cancel)?");
				if (fl == 0.0)
					break;
				if (fh >= fl)
					break;

				zf = iir(z, hz->npts, hz->delta, 2.0, fh, 2.0,
					 fl);
				nf = iir(n, hn->npts, hn->delta, 2.0, fh, 2.0,
					 fl);
				ef = iir(e, he->npts, he->delta, 2.0, fh, 2.0,
					 fl);

				if (rotated == ON)
					rotateNE2RT(hz->npts, ef, nf,
						    ((azimuth >
						      180.0) ? azimuth -
							 180.0 : azimuth +
						     180.0) * M_PI / 180.0);

				filteris = ON;
			} else {
				free(zf);
				zf = z;
				free(nf);
				nf = n;
				free(ef);
				ef = e;
				filteris = OFF;
				fh = SAC_HEADER_FLOAT_UNDEFINED;
				fl = SAC_HEADER_FLOAT_UNDEFINED;

				if (rotated == ON) {
					zf = malloc(sizeof(float) * hz->npts);
					nf = malloc(sizeof(float) * hn->npts);
					ef = malloc(sizeof(float) * he->npts);

					memcpy(zf, z, hz->npts * sizeof(float));
					memcpy(nf, n, hn->npts * sizeof(float));
					memcpy(ef, e, he->npts * sizeof(float));

					rotateNE2RT(hz->npts, ef, nf,
						    ((azimuth >
						      180.0) ? azimuth -
							 180.0 : azimuth +
							 180.0) * M_PI / 180.0);
				}
			}
			needsave = 0;
			break;
		case '1': // Mark P @ 0
			marks[0].set = 1;
			marks[0].time = ax;
			strcpy(marks[0].name, "P");
			break;
		case '2': // Mark S @ 1
			marks[1].set = 1;
			marks[1].time = ax;
			strcpy(marks[1].name, "S");
			break;
		case 'L':	// Read time
			if (mode == ZOOM) {
				SACTIME *t = getTimeStructFromSAC(hithead);
				int namplitude = hdu_getNptsFromSeconds(hithead, ax);
				namplitude = hdu_roundNPTS(hithead, namplitude);
				sumation(t, ax);
				char *tt = print_time(t, TIME_ISO);
				sprintf(statusline, "Pick @ %s (rel. is %.2f), Amplitude: %f",
					tt, ax, hitdata[namplitude]);
			}
			break;

		case 'P':	// Make a print - out
			cpgclos();
			cpgopen("azplot.ps/CPS");
			d(zc, nc, ec, azc, inc, zf, hz, nf, hn, ef, he, t1, t2,
			  azimuth, incidence, inct, needsave);
			cpgclos();
			i = opengr();
			resizemax(0.8);
			d(zc, nc, ec, azc, inc, zf, hz, nf, hn, ef, he, t1, t2,
			  azimuth, incidence, inct, needsave);
			sprintf(message, "Print wrote to: azplot.ps");
			alert(ANOUNCE);
			break;

			/* Marks related */
		case 'D':
			{	// Dump marks
				for (i = 0; i < nm; i++) {
					if (marks[i].set) {
						SACTIME *t =
						    getTimeStructFromSAC(hz);
						sumation(t, marks[i].time);
						char *tt =
						    print_time(t, TIME_ISO);
						fprintf(stdout, "%d %s %f %s\n",
							i, tt, marks[i].time,
							marks[i].name);
						free(tt);
						free(t);
					}
				}
			}
			break;

		case 'M':	// Mark
			if (mode == ZOOM) {
				int mark = -1;
				float mtime = ax;
				while (1) {
					mark = lerint("Which mark 0 to 10?");
					if (mark < 0 || mark >= nm)
						continue;
					break;
				}

				marks[mark].set = 1;
				marks[mark].time = mtime;
				lerchar("Phase name?", marks[mark].name, 5);

				needsave = 1;
			}
			break;

		case 'C':	// Clear Marks
			if (yesno("Clear all marks?")) {
				for (i = 0; i < nm; i++)
					marks[i].set = 0;

				needsave = 1;
			}
			break;

		case 'R':
			if (rotated == OFF) {
				if (filteris == OFF) {
					zf = malloc(sizeof(float) * hz->npts);
					nf = malloc(sizeof(float) * hn->npts);
					ef = malloc(sizeof(float) * he->npts);

					memcpy(zf, z, hz->npts * sizeof(float));
					memcpy(nf, n, hn->npts * sizeof(float));
					memcpy(ef, e, he->npts * sizeof(float));
				}

				rotateNE2RT(hz->npts, ef, nf,
					    ((azimuth >
					      180.0) ? azimuth -
						 180.0 : azimuth +
						 180.0) * M_PI / 180.0);
				rotated = ON;
				strcpy(nc->ylabel, "R");
				strcpy(ec->ylabel, "T");
				strcpy(azc->xlabel, "T");
				strcpy(azc->ylabel, "R");
			} else {
				free(zf);
				zf = z;
				free(nf);
				nf = n;
				free(ef);
				ef = e;
				rotated = OFF;
				strcpy(nc->ylabel, "N");
				strcpy(ec->ylabel, "E");
				strcpy(azc->xlabel, "EW");
				strcpy(azc->ylabel, "NS");

				if (filteris == ON) {
					zf = iir(z, hz->npts, hz->delta, 2.0,
						 fh, 2.0, fl);
					nf = iir(n, hn->npts, hn->delta, 2.0,
						 fh, 2.0, fl);
					ef = iir(e, he->npts, he->delta, 2.0,
						 fh, 2.0, fl);
				}
			}
			break;
			
		case 'S':
			hz->unused6 = hn->unused6 = he->unused6 = azimuth;
			hz->unused7 = hn->unused7 = he->unused7 = incidence;
			hz->unused8 = hn->unused8 = he->unused8 = t1;
			hz->unused9 = hn->unused9 = he->unused9 = t2;
			hz->unused10 = hn->unused10 = he->unused10 = fh;
			hz->unused11 = hn->unused11 = he->unused11 = fl;
			hz->unused15 = hn->unused15 = he->unused15 = inct;

			savemark(&marks[0],  hz, hn, he, "a", "ka");
			savemark(&marks[1],  hz, hn, he, "t0", "kt0");
			savemark(&marks[2],  hz, hn, he, "t1", "kt1");
			savemark(&marks[3],  hz, hn, he, "t2", "kt2");
			savemark(&marks[4],  hz, hn, he, "t3", "kt3");
			savemark(&marks[5],  hz, hn, he, "t4", "kt4");
			savemark(&marks[6],  hz, hn, he, "t5", "kt5");
			savemark(&marks[7],  hz, hn, he, "t6", "kt6");
			savemark(&marks[8],  hz, hn, he, "t7", "kt7");
			savemark(&marks[9],  hz, hn, he, "t8", "kt8");
			savemark(&marks[10], hz, hn, he, "t9", "kt9");
			savemark(&marks[11], hz, hn, he, "f", "kf");

			io_writeSacHead(zfilename, hz);
			io_writeSacHead(nfilename, hn);
			io_writeSacHead(efilename, he);
			needsave = 0;
			break;

        case 'H': {
            if (!marks[0].set || !marks[1].set){
                sprintf(message, "Need P and S marked (use '1' and '2' keys).");
                alert(0);
                break;
            }

            if (azimuth == SAC_HEADER_FLOAT_UNDEFINED) {
                sprintf(message, "Need a valid azimuth estimated.");
                alert(0);
                break;
            }

            if (hz->stla == SAC_HEADER_FLOAT_UNDEFINED || hz->stlo == SAC_HEADER_FLOAT_UNDEFINED){
                sprintf(message, "SAC data must have station coordinates.");
                alert(0);
                break;
            }

            float sp=marks[1].time - marks[0].time;
            float distance = time2distance(sp);
            if (distance == -999.0) {
                sprintf(message, "S-P time is not in table.");
                alert(0);
                break;
            }

            float elon, elat;
            model_locate(hz->stlo, hz->stla, distance, azimuth, &elon, &elat);

            hz->evla = hn->evla = he->evla = elat;
            hz->evlo = hn->evlo = he->evlo = elon;
            hz->evdp = hn->evdp = he->evdp = 0.0;
            hz->dist = hn->dist = he->dist = distance;
            hz->gcarc = hn->gcarc = he->gcarc = distance / 111.32;
            hz->baz   = hn->baz   = he->baz  = azimuth;
            hz->lcalda = hn->lcalda = he->lcalda = SAC_FALSE;

            if (yesno("Open Google to show location?") == 1) {
                char cmd_line[10500];
                sprintf(cmd_line, "x-www-browser 'http://www.google.com/maps/place/%.4f,%.4f/@%.4f,%.4f'",elat,elon,elat,elon);
                system(cmd_line);
            }
        }

        case 'Q': {
			if (needsave) {
				int yn = yesno("File will not be saved, edits will be lost, really quit?");
				ch = (yn == 1) ? ch : ' ';
			}
			break;
		}

		default:
			printf("Oops, invalid command.\n");
			break;
		}
	}
	
	return;
}

int main(int argc, char **argv)
{
    int needsave, force = 0;
	SACHEAD *hz, *hn, *he;
	float *z, *n, *e;
	int stop = 0;
	char zfilename[2048] = "", nfilename[2048] = "", efilename[2048] = "";
	
	if (argc < 2) {
		fprintf(stderr, "Invalid number of traces.\n");
		return -1;
	}
	
	hz = hn = he = NULL;
	z = n = e = NULL;
	needsave = 0;

	int i = 1;
	
	while (i<argc) {
		if (strncmp(argv[i], "-f",2)==0) {
			force = 1;
			i++;
			continue;
		}

		if (strlen(zfilename) == 0) {
			sprintf(zfilename, "%s",argv[i]);
		} else if (strlen(nfilename) == 0) {
			sprintf(nfilename, "%s",argv[i]);
		} else if (strlen(efilename) == 0) {
			sprintf(efilename, "%s",argv[i]);
		}
		
		i++;
	}
	
	if (strlen(zfilename) == 0) {
		fprintf(stderr, "Error, no file was provided.\n");
		return -1;
	}
	
	if ((strlen(efilename) == 0) && (strlen(efilename) == 0)) {
		sprintf(nfilename, "%s.n",zfilename);
		sprintf(efilename, "%s.e",zfilename);
		sprintf(zfilename, "%s.z",zfilename);
	}
	
	if ((strlen(efilename) == 0) || (strlen(efilename) == 0)) {
		fprintf(stderr, "Error, no one base filename, or 3 (ZNE) files in order.\n");
		return -1;
	}
		
	z = io_readSac(zfilename, &hz);
	if (z == NULL) {
		fprintf(stderr, "Error reading z file: %s\n", zfilename);
		stop = 1;
	}
	
	n = io_readSac(nfilename, &hn);
	if (n == NULL) {
		fprintf(stderr, "Error reading n file: %s\n", nfilename);
		stop = 1;
	}
	
	e = io_readSac(efilename, &he);
	if (e == NULL) {
		fprintf(stderr, "Error reading e file: %s\n", efilename);
		stop = 1;
	}
	
	/* check that files reference and sps match ! */
	if (hz->cmpinc != 0.0 || hn->cmpaz != 0.0 || he->cmpaz != 90.0) {
		fprintf(stderr, "Components orientation do not match.\n");
		if (!force) stop = 1;
	}
	
	if (!stop) {
		if (hz->nzyear != he->nzyear ||
				hz->nzjday != he->nzjday ||
				hz->nzmin != he->nzmin ||
				hz->nzhour != he->nzhour ||
				hz->nzsec != he->nzsec ||
				hz->nzmsec != he->nzmsec ||
				hz->b != he->b || hz->delta != he->delta
				|| hz->npts != he->npts) {
			stop = 1;
			fprintf(stderr,
					"Z and E files reference time differ.\n");
		}
		
		if (hz->nzyear != hn->nzyear ||
				hz->nzjday != hn->nzjday ||
				hz->nzmin != hn->nzmin ||
				hz->nzhour != hn->nzhour ||
				hz->nzsec != hn->nzsec ||
				hz->nzmsec != hn->nzmsec ||
				hz->b != hn->b || hz->delta != hn->delta
				|| hz->npts != hn->npts) {
			stop = 1;
			fprintf(stderr,
					"Z and N files reference time differ.\n");
		}
	}
	
	if (!stop) {
		yu_rtrend(z, hz->npts);
		yu_rtrend(n, hn->npts);
		yu_rtrend(e, he->npts);
		
		if (z == NULL)
			fprintf(stderr, "OILA");
		interact(zfilename, z, hz, nfilename, n, hn, efilename, e, he);
	}
	
	if (z != NULL) {
		free(z);
		z = NULL;
		free(hz);
		hz = NULL;
	}
	
	if (n != NULL) {
		free(n);
		n = NULL;
		free(hn);
		hn = NULL;
	}
	
	if (e != NULL) {
		free(e);
		e = NULL;
		free(he);
		he = NULL;
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
