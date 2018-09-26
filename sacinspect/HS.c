#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>

#include <gr.h>
#include <collector.h>
#include <math.h>

#define RES 0
#define DIST 1
#define RDIST 2

void realhistogram(float *x, int n, float Smin, float Smax, int nbin)
{
	float min, max;
	char string[500];
	int i;

	if (n == 0)
		return;
	if (nbin == 0)
		return;

	min = max = x[0];
	for (i = 0; i < n; i++) {
		if (x[i] < min)
			min = x[i];
		if (x[i] > max)
			max = x[i];
	}

	if (Smin == Smax)
		cpghist(n, x, min, max, nbin, 4);
	else
		cpghist(n, x, Smin, Smax, nbin, 4);

	sprintf(string, "%.1f/%.1f", min, max);
	cpgmtxt("R", 1.5, 0.5, 0.5, string);
}

void makehistorgram(pick ** plist, int n, float Smin, float Smax, int nbin,
					int type)
{
	float *x;
	int i;

	x = malloc(sizeof(float) * n);

	switch (type) {
	case (0):
		for (i = 0; i < n; i++)
			x[i] = plist[i]->residual;
		break;

	case (1):
		for (i = 0; i < n; i++) {
			float Std = ((event *) (plist[i]->ev))->resStd;
			x[i] = fabs(plist[i]->residual) / Std;
		}
		break;
	}

	realhistogram(x, n, Smin, Smax, nbin);

	free(x);
	x = NULL;
}

void drawaz(float size, float interval, float anot)
{
	int i;
	char string[500];

	cpgsfs(2);
	cpgenv(-size, size, -size, size, 1, -2);

	if (interval > 0)
		for (i = 1; (i * interval) < size; i++)
			cpgcirc(0.0, 0.0, (float) i * interval);

	if (anot > 0 )
		for (i = 1; (i * anot) < size; i++) {
			sprintf(string, "%.2f", i * anot);
			cpgptxt(-i * anot * 1.03 * sin(45 * M_PI / 180),
					i * anot * 1.03 * cos(45 * M_PI / 180), 45.0, 0.5, string);
		}

	cpgcirc(0.0, 0.0, size);
	sprintf(string, "%.2f", size);
	cpgptxt(-size * 1.04 * sin(45 * M_PI / 180),
			size * 1.04 * cos(45 * M_PI / 180), 45.0, 0.5, string);

	cpgmove(0.0, -size);
	cpgdraw(0.0, size);

	cpgmove(-size, 0.0);
	cpgdraw(size, 0.0);

	cpgsch(1.0);
	cpgptxt(0.0, size * 1.04, 0.0, 0.5, "N");
	cpgptxt(0.0, -(size * 1.08), 0.0, 0.5, "S");
	cpgptxt( size, 0.0 - 2.0, 0.0, -0.6, "E");
	cpgptxt(-size, 0.0 - 2.0, 0.0, +1.6, "W");
	cpgsch(0.675);
}

void detail(pick **plist, int n, char *stname)
{
	char ch = '-';
	float ay;
	int i, mode = DIST;

	cpgeras();
	while (ch != 'q') {
		cpgsubp(2, 1);

		cpgsch(0.675);
		makehistorgram(plist, n, -4, 4, 31, 0);
		
		if (mode == DIST) {
			drawaz(120.0, 15.0, 30.0);
		} else if (mode == RES) {
			drawaz(3.0, 0.5, 1.0);
		} else if (mode == RDIST) {
			drawaz(1.0/25.0, 1/100.0, 1/50.0);
		}
		
		cpgsch(1.0);
		cpgmtxt("T", 3.5, 0.5, 0.5, stname);
		cpgmtxt("T", 2.0, 0.5, 0.5, (mode == RES)?"Residual Mode":(mode == DIST)?"Distance Mode":"Inverse Distance Mode");
		cpgsch(0.675);
		
		for (i = 0; i < n; i++) {
			float x; 
			float y;
			
			if (mode == DIST) {
				x = plist[i]->gcarc*sin(plist[i]->baz*M_PI/180.0);
				y = plist[i]->gcarc*cos(plist[i]->baz*M_PI/180.0);
			} else if (mode == RES) {
				x = fabs(plist[i]->residual) * sin(plist[i]->baz * M_PI / 180.0);
				y = fabs(plist[i]->residual) * cos(plist[i]->baz * M_PI / 180.0);
			} else if (mode == RDIST) {
				x = (1.0 / plist[i]->gcarc)*sin(plist[i]->baz*M_PI/180.0);
				y = (1.0 / plist[i]->gcarc)*cos(plist[i]->baz*M_PI/180.0);
			}
			
			int color = 1;
			
			if (plist[i]->residual < -0.2)
				color = 4;
			
			if (plist[i]->residual > 0.2)
				color = 2;
			
			cpgsci(color);
			cpgsch(1.2);
			cpgpt1(x, y, -3);
			cpgsci(1);
		}
		
		ch = getonechar(&ay, &ay);
		if (ch == 'i') mode = RDIST;
		if (ch == 'd') mode = DIST;
		if (ch == 'r') mode = RES;
	}
	return;
}

void EXHIST_process(glob_t * glb)
{
	stations *ss;
	events *evs;
	int n;
	long pCount = 0;
	int panel, sta;

	float ay, ax;
	char ch = '-';

	ss = newStationList(glb);
	evs = newEventList(glb, ss, 0);

	for (n = 0; n < evs->n; n++)
		pCount += evs->elist[n]->n;

	if (collectorVerbose)
		fprintf(stderr, "\n");
	
	fprintf(stderr, "Total of %d Stations collected.\n", ss->n);
	fprintf(stderr, "Total of %d Events collected.\n", evs->n);
	fprintf(stderr, "Total of %ld Picks collected.\n", pCount);

	opengr();
	
	int offset = 0;
	while (ch != 'q') {
		float sigma = 3.0;
		cpgsch(2.5);
		cpgsubp(10, 8);
		long totaln = 0;
		int max = 80;
		for (panel = 0;
			 ((panel < max)) && ((panel + offset)) < ss->n;
			 panel++) {
			sta = offset + panel;

			// Collect
			pick **plist =
				collectpicksbystation(evs, ss, sta, &n, 0, sigma);
			totaln += n;
			
			if (plist == NULL) {
				cpgenv(0.0, 1.0, 0.0, 1.0, 0, 0);
				cpgmtxt("T", 3.5, 0.5, 0.5, "No Data for Station");
			} else {
				// Plot
				if (collectorVerbose)
					fprintf(stderr, "Hist %d (%s %s)\n", sta, ss->slist[sta]->net, ss->slist[sta]->name);
				makehistorgram(plist, n, -4.0, 4.0, 31.0, 0);

				cpgsch(3);
				if (panel % 10 == 0)
					cpgmtxt("L", 2.5, 0.5, 0.5, "Count");

				if (panel >= 50)
					cpgmtxt("B", 3.0, 0.5, 0.5, "Res");

				cpgsch(4);
				cpgmtxt("T", 0.65, 0.5, 0.5, ss->slist[sta]->name);

				// Clear
				if (plist != NULL)
					free(plist);
				plist = NULL;
				cpgsch(2.5);
			}
		}
		
		cpgsubp(1, 1);
		cpgsch(1.0);
		cpgsvp(0.0, 1.0, 0.0, 1.0);
		cpgswin(0.0, 10.0, 0.0, 8.0);
		ch = getonechar(&ax, &ay);

		if (ch != 'q') {
			int col = (int) ax;
			int line = 7 - (int) ay;
			sta = (line * 10 + col);
			pick **plist =
				collectpicksbystation(evs, ss, sta, &n, 0, sigma);
			detail(plist, n, ss->slist[sta]->name);
			free(plist);
			plist = NULL;
		}

	}

	cpgclos();

	killStations(ss);
	killEvents(evs);
}
