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

#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <sac.h>
#include <cpgplot.h>
#include <gr.h>
#include <string.h>
#include <cfg.h>

#ifndef _lala_
#define _lala_

#define ALIGA 0
#define ALIGF 1
#define ALIGO 2

// Config variable names
#define NAME_HP "highPass"
#define NAME_LP "lowPass"
#define NAME_PATTERN "folderPattern"
#define NAME_PRE "preWindow"
#define NAME_POST "postWindow"

// Config default values
#define DEFAULT_PATTERN "??????_????"
#define DEFAULT_LP 2.0
#define DEFAULT_HP 0.5
#define DEFAULT_PRE 5.0
#define DEFAULT_POST 20.0

typedef struct timefile {
	double reference;
	char *filename;
	char *station;
	char *net;

	SACHEAD *hz, *hn, *he;
	float *z, *n, *e;
	float *zf, *nf, *ef;

	int selected;
} tf;

typedef struct {
	// Plot Controls
	int grID;

	g_ctl **ctl;
	int hidephase;
	int putname;
	int printout;
	int alig;
	int offset;
	int max;
	float gpre;
	float gpost;
	float prephase;
	float postphase;
	int overlay;
	char lastaction[1024];
	int onlyselected;
	int plotsearchsize;
	float zoom;
	int sortmode;

	// Filters
	float lp;
	float hp;
	int filter;
	int needfilter;

	// Data
	glob_t *glb;
	int has;
	tf *files;
	int nfiles;
	int currentdir;

	// Processing parameters
	float searchsize;
	float insetsearchsize;

	int needsave;
} defs;

typedef struct {
	float lp;
	float hp;
	float gpre;
	float gpost;
} globconf;

extern CFG *config;
#endif
