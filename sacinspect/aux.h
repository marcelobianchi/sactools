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

void multitraceplot(defs * d);
void getminmax(float *data, SACHEAD * hdr, float start, float end,
			   float *rmin, float *rmax);
void filtertf(tf * f, defs * d);
void writeout(tf * f, defs * d);
void loadtf(tf * f);
void synch(tf * f, defs * d);
void killCTL(g_ctl *** c, int n);
int cmpstringp(const void *p1, const void *p2);
glob_t *dirlist(char *pattern);
glob_t *saclist(defs * d);
tf *inputme(int argc, char **argv, defs * d);
g_ctl **initCTL(defs * d);
defs *newdefs(glob_t * glb);
void tffree(tf * tf, int n);
void plot(g_ctl * ctl, float *data, int npts, float delta, float b,
		  int overlay);

int sortDist(const void *p1, const void *p2);
int sortBaz(const void *p1, const void *p2);
