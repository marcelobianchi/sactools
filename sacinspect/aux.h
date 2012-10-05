/*
    This file is part of sactools package.
    
    sactools is a package to managing and do simple processing with SAC
    seismological files.
    Copyright (C) 2012  Marcelo B. de Bianchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <inspect.h>

void checkTREF(tf * files, int nfiles);
void multitraceplot(defs * d);
void getminmax(float *data, SACHEAD * hdr, float start, float end,
			   float *rmin, float *rmax);
void filtertf(tf * f, defs * d);
void writeout(tf * f, defs * d);
void synch(tf * f, int nfiles);
void killCTL(g_ctl *** c, int n);
int cmpstringp(const void *p1, const void *p2);

g_ctl **initCTL(defs * d);
defs *newdefs(glob_t * glb);
void tffree(tf * tf, int n);
void plot(g_ctl * ctl, float *data, int npts, float delta, float b,
		  int overlay);
int sortDist(const void *p1, const void *p2);
int sortBaz(const void *p1, const void *p2);


/* Picker handling methods */
pdefs *killpicker(pdefs *pick);
pdefs *newpicker(PickTypes picktype, char *from, char *to,  char *name);
void pickLoadPhase(pdefs *pick, char *phase);
float pickD(pdefs *pick, SACHEAD *head);
float pickR(pdefs *pick, SACHEAD *head);
void setPick(pdefs *pick, SACHEAD *head, float value);
