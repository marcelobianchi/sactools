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

#include<stdio.h>
#include<stdlib.h>

#include<inspect.h>
#include<head.h>
#include<edit_tf.h>

#include<timeu.h>

void plothelp(defs * d)
{
	int k = 0;
	float where = -0.01;
	float how = 1.0;

	if (d->printout == 0) {
		/* Write the last action */
		if (strlen(d->lastaction)) {
			cpgsci(14);
			cpgmtxt("B", 2.5, 0.5, 0.5, d->lastaction);
			cpgsci(1);
			strcpy(d->lastaction, "");
		}
		// Commands sumary
		cpgsci(14);
		cpgsch(.65);
		k--;
		cpgmtxt("B", k--, where, how, "Only Sel Sp");
		cpgmtxt("B", k--, where, how, "Names  t");
		cpgmtxt("B", k--, where, how, "Overlay +");
		cpgmtxt("B", k--, where, how, "A Mark  #");
		cpgmtxt("B", k--, where, how, "View Sw m");
		cpgmtxt("B", k--, where, how, "Searc Ev -");
		cpgmtxt("B", k--, where, how, "Search  :");
		cpgmtxt("B", k--, where, how, "Print o");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "General:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Post Sw s");
		cpgmtxt("B", k--, where, how, "Pre Sw i");
		cpgmtxt("B", k--, where, how, "Config ?");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "Config:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Low  l");
		cpgmtxt("B", k--, where, how, "High h");
		cpgmtxt("B", k--, where, how, "Filter On f");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "Filter:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Max 1");
		cpgmtxt("B", k--, where, how, "Min 2");
		cpgmtxt("B", k--, where, how, "Clean 3");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "All:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Max y|z");
		cpgmtxt("B", k--, where, how, "Min x");
		cpgmtxt("B", k--, where, how, "Clean c");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "Current:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Delete d");
		cpgmtxt("B", k--, where, how, "Edit   e");
		k--;
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "Picking Keys:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Write w");
		cpgmtxt("B", k--, where, how, "Reload r");
		cpgmtxt("B", k--, where, how, "Quit Q");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "IO:");
		cpgsci(14);

		k--;
		cpgmtxt("B", k--, where, how, "Ampl. Zoom !");
		cpgmtxt("B", k--, where, how, "+ Window *");
		cpgmtxt("B", k--, where, how, "- Window /");
		k--;
		cpgmtxt("B", k--, where, how, "Sort 0");
		cpgmtxt("B", k--, where, how, "Align .");
		cpgmtxt("B", k--, where, how, "Tr. per Sc. ,");
		k--;
		cpgmtxt("B", k--, where, how, "Event N,P");
		cpgmtxt("B", k--, where, how, "Trace n,p");
		cpgsci(15);
		cpgmtxt("B", k--, where, how, "Control:");
		cpgsci(14);

		cpgsci(1);
		cpgsch(.65);
	}
}

void plot(g_ctl * ctl, float *y, int npts, float delta, float b,
		  int overlay, int color, float zoom)
{
	float *x = NULL;
	int i;
	int oldcolor;

	cpgqci(&oldcolor);

	x = malloc(sizeof(float) * npts);
	for (i = 0; i < npts; i++)
		x[i] = i * delta + b;

	if (!overlay) {
		ctl_yupdate_ndb(ctl, y, npts, delta, b);

		ctl->ymin /= zoom;
		ctl->ymax /= zoom;
		ctl->h = ctl->ymax - ctl->ymin;

		ctl_resizeview(ctl);
		ctl_drawaxis(ctl);
	}

	cpgsci(color);
	cpgline(npts, x, y);
	cpgsci(oldcolor);

	x = io_freeData(x);
}


void filtertf(tf * f, defs * d)
{
	if (f->z == NULL) {
		f->zf = NULL;
		return;
	}

	f->zf = io_freeData(f->zf);

	if (d->filter) {
		yu_rtrend(f->z, f->hz->npts);
		f->zf = iir(f->z, f->hz->npts, f->hz->delta,
					(d->hp > 0.0) ? 2 : 0, d->hp, (d->lp > 0.0) ? 2 : 0,
					d->lp);

		if (f->zf == NULL) {
			strcpy(message, "Something wrong with the filter.");
			alert(WARNING);
		}
	}

	return;
}


void multitraceplot(defs * d)
{
	int j;
	float start;
	char string[1024];
	g_ctl **ctl = d->ctl;
	int color = 1;
	char aligChar;
	float x1, x2;
	int lastFile;

	ctl_clean(NULL);

	ctl_resizeview(ctl[0]);
	plothelp(d);

	// If no traces found give warning and return
	if (d->nfiles <= 0) {
		sprintf(string,
				"No folders matching pattern found. Use ? to change folder pattern configuration !");
		cpgmtxt("B", 0.5, 0.5, 0.5, string);
		return;
	}

	lastFile = ((d->offset + d->max) >=
				d->nfiles) ? (d->nfiles - 1) : (d->offset + d->max);

	// Those are used for min and max of the ALIGO mode
	x1 = d->files[d->offset].hz->a + d->files[d->offset].reference -
		2.5;
	x2 = d->files[lastFile].hz->a + d->files[lastFile].reference +
		d->postphase;
	for (j = 0; j < d->max && (j + d->offset) < d->nfiles; j++) {

		/* *********************************************************** */
		/* Prepare the file to plot                                    */
		/* *********************************************************** */
		tf *this = &d->files[j + d->offset];
		if (d->onlyselected && !this->selected)
			continue;

		if (d->filter && d->needfilter) {
			filtertf(this, d);
		}

		/* *********************************************************** */
		/* Switch Aligment mode                                        */
		/* *********************************************************** */

		(j == 0) ? ctl_axisbottom(ctl[j]) : ctl_axisnone(ctl[j]);
		switch (d->alig) {
		case (ALIGO):
			aligChar = 'O';
			if (d->files[0].hz->o != SAC_HEADER_FLOAT_UNDEFINED) {
				start = this->reference - d->files[0].hz->o;
				ctl_xreset_mm(ctl[j], x1 - d->files[d->offset].hz->o,
							  x2 - d->files[d->offset].hz->o);
			} else {
				start = this->reference;
				ctl_xreset_mm(ctl[j], x1, x2);
				aligChar = '!';

				SACTIME *time = getTimeStructFromSAC(d->files[0].hz);
				char *stime = print_time(time, TIME_ISO);
				sprintf(d->lastaction, "Reference is: %s", stime);
				stime = io_freeData(stime);
				time = io_freeData(time);
			}
			break;

		case (ALIGF):
			aligChar = 'F';
			ctl_xreset_mm(ctl[j], -d->prephase, +d->postphase);
			if (this->hz->f != SAC_HEADER_FLOAT_UNDEFINED) {
				start = this->hz->b - this->hz->f;
			} else {
				start = this->hz->b;
			}
			break;

		case (ALIGA):
			aligChar = 'A';
			ctl_xreset_mm(ctl[j], -d->prephase, +d->postphase);
			if (this->hz->a != SAC_HEADER_FLOAT_UNDEFINED) {
				start = this->hz->b - this->hz->a;
			} else {
				start = this->hz->b;
			}
			break;
		}

		/* *********************************************************** */
		/* Plot the trace (DATA)                                       */
		/* *********************************************************** */

		// Selected color
		color = 14;				// Default trace color
		if (this->selected == 1)
			color = 2;			// If event Selected Color = Red
		// if (this->hz->unused25 != 1) color = 14; // If we are in the restricted mode and trace is locked Color = Gray
		if (d->overlay)
			color = j + 1;

		// Finally Plot
		plot(ctl[j], (d->filter && this->zf != NULL) ? this->zf : this->z,
			 this->hz->npts, this->hz->delta, start, 0, color, d->zoom);
		/* *********************************************************** */

		/* *********************************************************** */
		/* Plot MARKS & Names                                          */
		/* *********************************************************** */

		// F mark, the pick time
		if (this->hz->f != SAC_HEADER_FLOAT_UNDEFINED)
			mark(ctl[j], this->hz->f + start - this->hz->b, "f", 2);

		// IF OVERLAY MODE JUST PLOT THE TRACE AND RETURN.
		if (d->overlay == 1)
			continue;

		// A Mark (Theoretical Model)
		if (this->hz->a != SAC_HEADER_FLOAT_UNDEFINED && d->hidephase == 0) {
			char *label = NULL;

			hdu_getValueFromChar("ka", this->hz, NULL, NULL, &label);
			if (label != NULL && strlen(label) > 0)
				mark(ctl[j], this->hz->a + start - this->hz->b, label, 2);
			else
				mark(ctl[j], this->hz->a + start - this->hz->b, "A", 2);

			label = io_freeData(label);
		}
		// Mark the Window we perform the Min/Max Search in the trace
		if (d->plotsearchsize) {
			mark(ctl[j], this->hz->a + start - this->hz->b + d->searchsize,
				 "", 3);
			mark(ctl[j],
				 this->hz->a + start - this->hz->b - d->insetsearchsize,
				 "", 3);
		}

		sprintf(string, "B: %.0f D: %.2f", this->hz->baz, this->hz->gcarc);
		if (d->putname == 2)
			cpgmtxt("B", -2.00, 0.0, 0.0, string);
		else
			cpgmtxt("B", -0.70, 0.0, 0.0, string);


		// Add the name of the trace
		if (d->putname > 0) {
			cpgsci(2);

			cpgmtxt("R", 1.2, .5, .5, this->net);
			if (d->max <= 20)
				cpgmtxt("R", 2.2, .5, .5, this->station);

			if (d->putname == 2) {
				sprintf(string, "%02d] %s", d->offset + j, this->filename);
				cpgmtxt("B", -0.70, 0.0, 0.0, string);
			}
			cpgsci(1);
		}

	}							// End of trace for loop

	// basic information on the foot
	ctl_resizeview(ctl[0]);
	sprintf(string, "[%03d-%03d/%03d] [Filter %s (%.2f to %.2f hz)]",
			d->offset + 1, d->offset + d->max, d->nfiles,
			(d->filter) ? "ON" : "OFF", d->hp, d->lp);
	cpgmtxt("B", 2.5, 0.0, 0.0, string);

	sprintf(string, "[%c] [PSw: %.1f ISw: %.1f Tr/S: %d] [%s]", aligChar,
			d->searchsize, d->insetsearchsize, d->max,
			(d->sortmode == 0) ? "DZ" : "BAZ");
	cpgmtxt("B", 2.5, 1.0, 1.0, string);

	ctl_resizeview(ctl[d->max - 1]);
	sprintf(string, "%11s",
			(d->currentdir - 1 >=
			 0) ? d->glb->gl_pathv[d->currentdir - 1] : "First Directory");
	cpgmtxt("T", 1.0, 0.35, 1.0, string);

	sprintf(string, "%11s",
			(d->currentdir + 1 <
			 d->glb->gl_pathc) ? d->glb->gl_pathv[d->currentdir +
												  1] : "Last Directory");
	cpgmtxt("T", 1.0, 0.65, 0.0, string);

	sprintf(string, "%11s", d->glb->gl_pathv[d->currentdir]);
	if (d->needsave == 1) {
		cpgsci(2);
	} else {
		cpgsci(3);
	}
	cpgmtxt("T", 1.0, 0.5, 0.5, string);
	cpgsci(1);
}

void writeout(tf * files, defs * d)
{
	tf *f;
	char string[1025];
	int j;

	for (j = 0; j < d->nfiles; j++) {
		f = &files[j];
		if (f != NULL && f->hz != NULL && f->filename != NULL) {
			f->hz->user0 = d->lp;
			f->hz->user1 = d->hp;
			io_writeSacHead(f->filename, f->hz);
		}
	}
	d->needsave = 0;

	FILE *a;
	sprintf(string, "%s/HAS", d->glb->gl_pathv[d->currentdir]);
	a = fopen(string, "w");
	fclose(a);
}

void getminmax(float *data, SACHEAD * hdr, float start, float end,
			   float *rmin, float *rmax)
{
	float min, max;
	double mean = 0.0;
	int jmin, jmax, count, jstart, jend;
	int j;

	*rmin = SAC_HEADER_FLOAT_UNDEFINED;
	*rmax = SAC_HEADER_FLOAT_UNDEFINED;

	jstart = hdu_getNptsFromSeconds(hdr, start);
	jend = hdu_getNptsFromSeconds(hdr, end);

	jstart = hdu_roundNPTS(hdr, jstart);
	jend = hdu_roundNPTS(hdr, jend);

	if (jend <= jstart) {
		return;
	}

	jmax = jmin = jstart;
	mean = max = min = data[jstart];
	count = 1;

	for (j = jstart + 1; j <= jend; j++) {
		mean += data[j];
		count++;
		if (data[j] > max) {
			max = data[j];
			jmax = j;
		}
		if (data[j] < min) {
			min = data[j];
			jmin = j;
		}
	}

	mean = mean / count;
	max -= mean;
	min -= mean;

	*rmax = hdu_getSecondsFromNPTS(hdr, jmax);
	*rmin = hdu_getSecondsFromNPTS(hdr, jmin);
}

void loadtf(tf * f)
{
	if (f->z != NULL) {
		f->z = io_freeData(f->z);
	}

	if (f->zf != NULL) {
		f->zf = io_freeData(f->zf);
	}

	f->z = io_readSac(f->filename, &f->hz);
	if (f->z == NULL) {
		fprintf(stderr, "Failed to load: %s\n", f->filename);
		exit(0);
	}

	hdu_getValueFromChar("KSTNM", f->hz, NULL, NULL, &f->station);
	hdu_getValueFromChar("KNETWK", f->hz, NULL, NULL, &f->net);

	f->selected = 0;
}

void synch(tf * f, defs * d)
{
	tf *ref;
	int j;


	//    c<-b2>d------/
	//  a<---b1-->b----/--
	// REF = a to c + b2
	// for the first trace (reference one) is a to a + b1

	ref = &f[0];
	ref->reference = ref->hz->b;

	for (j = 1; j < d->nfiles; j++) {
		tf *t = &f[j];

		if (ref->hz->nzyear != t->hz->nzyear) {
			t->reference = 0.0;
			continue;
		}

		double val = (t->hz->nzjday - ref->hz->nzjday) * 24 * 60 * 60 +
			(t->hz->nzhour - ref->hz->nzhour) * 60 * 60 +
			(t->hz->nzmin - ref->hz->nzmin) * 60 +
			(t->hz->nzsec - ref->hz->nzsec) +
			(t->hz->nzmsec - ref->hz->nzmsec) / 1000.0 + t->hz->b;

		t->reference = val;
	}
}

int sortDist(const void *p1, const void *p2)
{
	tf *t1 = (tf *) p1;
	tf *t2 = (tf *) p2;

	return (t1->hz->gcarc) > (t2->hz->gcarc);
}

int sortBaz(const void *p1, const void *p2)
{
	tf *t1 = (tf *) p1;
	tf *t2 = (tf *) p2;

	return (t1->hz->baz) > (t2->hz->baz);
}

glob_t *dirlist(char *pattern)
{
	glob_t *glb = NULL;

	glb = malloc(sizeof(glob_t));
	glb->gl_pathc = 0;
	glob(pattern, GLOB_ONLYDIR, NULL, glb);

	return glb;
}

void * killGlob(glob_t *glb){
	if (glb != NULL) {
		globfree(glb);
		free(glb);
	}
	return NULL;
}

void checkTREF(tf * files, int nfiles)
{
	int has = 1;
	char phase[4];
	char kphase[10];
	int i;

	for (i = 0; i < nfiles; i++)
		if (files[i].hz->a == SAC_HEADER_FLOAT_UNDEFINED)
			has = 0;

	if (has == 1)
		return;

	lerchar
		("Not all your files has a pick, enter the name of header to import picks or none to quit?",
		 phase, 4);

	SACHEADDEF *hdef = getSacHeadDefFromChar(phase);
	if (hdef == NULL || hdef->isMark != 1) {
		strcpy(message, "Okay. Skipping the import of reference phases.");
		alert(WARNING);
		return;
	}

	sprintf(kphase, "K%s", phase);

	for (i = 0; i < nfiles; i++) {
		float val;
		char *name = NULL;

		// Get
		hdu_getValueFromChar(phase, files[i].hz, &val, NULL, NULL);
		hdu_getValueFromChar(kphase, files[i].hz, NULL, NULL, &name);

		// Check
		if (val == SAC_HEADER_FLOAT_UNDEFINED) {
			strcpy(message,
				   "The desired header variable is also undefined.");
			alert(WARNING);
			continue;
		}
		// Set
		files[i].hz->a = val;
		changeCharValueFromChar(files[i].hz, kphase, name);
	}
}

glob_t *saclist(defs * d)
{
	glob_t *glb = NULL;
	char *filepath = NULL;
	char *path = NULL;
	FILE *a = NULL;

	glb = malloc(sizeof(glob_t));
	glb->gl_pathc = 0;

	if (d->glb->gl_pathc == 0)
		return glb;

	path = d->glb->gl_pathv[d->currentdir];
	filepath = malloc(sizeof(char) * (strlen(path) + 10));
	sprintf(filepath, "%s/*SAC", path);
	glob(filepath, 0, NULL, glb);

	sprintf(filepath, "%s/HAS", path);
	if ((a = fopen(filepath, "r")) != NULL) {
		d->has = 1;
		fclose(a);
	} else {
		d->has = 0;
	}

	free(filepath);
	filepath = NULL;
	return glb;
}

tf *inputme(int argc, char **argv, defs * d)
{
	tf *files = NULL;
	int i;

	d->nfiles = 0;
	d->offset = 0;

	d->alig = (d->has) ? ALIGF : ALIGA;
	d->lp = getConfigAsNumber(config, NAME_LP, DEFAULT_LP);
	d->hp = getConfigAsNumber(config, NAME_HP, DEFAULT_HP);

	if (argc == 0)
		return files;

	files = malloc(sizeof(tf) * argc);

	for (i = 0; i < argc; i++) {
		d->nfiles++;
		files[i].z = NULL;
		files[i].zf = NULL;

		files[i].filename = malloc(sizeof(char) * (strlen(argv[i]) + 1));
		strcpy(files[i].filename, argv[i]);

		loadtf(&files[i]);
		if (files[i].z == NULL)
			return NULL;
	}

	for (i = 0; i < argc; i++) {
		if (files[i].hz->user0 != -12345.0
			&& files[i].hz->user0 > files[i].hz->user1) {
			d->lp = files[i].hz->user0;
			d->hp = files[i].hz->user1;
			d->filter = ((d->lp != 0.0) || (d->hp != 0.0));
			break;
		}
	}

	if (d->filter)
		for (i = 0; i < argc; i++)
			filtertf(&files[i], d);

	/* Refine max */
	/* for (i = 0; i < argc; i++){ */
	/*   float pos; */
	/*   pos = files[i].hz->f; */
	/*   if (pos != SAC_HEADER_FLOAT_UNDEFINED){ */
	/*     pos = minmaxrefine(&files[i], d, pos, 0.5); */
	/*     fprintf(stderr,"%f\n", pos - files[i].hz->f); */
	/*     if (fabs(pos - files[i].hz->f)<0.5) files[i].hz->f=pos; */
	/*   } */
	/* } */
	/* fprintf(stderr,"\n"); */

	// Check that we have picks
	checkTREF(files, d->nfiles);

	// Sort the files 
	qsort(files, d->nfiles, sizeof(tf), sortDist);

	// Synch Time
	synch(files, d);

	return files;
}

void tffree(tf * tf, int n)
{
	int i;

	if (tf == NULL)
		return;

	for (i = 0; i < n; i++) {
		if (tf[i].filename != NULL)
			free(tf[i].filename);
		if (tf[i].zf != NULL)
			free(tf[i].zf);
		if (tf[i].z != NULL)
			free(tf[i].z);
		if (tf[i].hz != NULL)
			free(tf[i].hz);
		if (tf[i].station != NULL)
			free(tf[i].station);
		if (tf[i].net != NULL)
			free(tf[i].net);

		tf[i].filename = NULL;
		tf[i].z = NULL;
		tf[i].zf = NULL;
		tf[i].hz = NULL;
		tf[i].net = NULL;
		tf[i].station = NULL;
	}

	free(tf);
	return;
}

void killCTL(g_ctl *** ci, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		free((*ci)[i]);
		(*ci)[i] = NULL;
	}
	free((*ci));
	(*ci) = NULL;
}

g_ctl **initCTL(defs * d)
{
	g_ctl **ctl;
	int i;

	ctl = malloc(sizeof(g_ctl *) * (d->max));
	float y = 0.05;
	float ysize = 0.9 / ((double) d->max);
	for (i = 0; i < d->max; i++) {
		if (d->overlay == 0)
			ctl[i] = ctl_newctl(0.08, y, 0.88, ysize);
		else
			ctl[i] = ctl_newctl(0.08, 0.05, 0.88, 0.9);

		ctl_axisnone(ctl[i]);
		y += ysize;
	}

	return ctl;
}

defs *newdefs(glob_t * glb)
{
	defs *d;
	d = calloc(1,sizeof(defs));

	d->printout = 0;
	d->putname = 0;
	d->alig = ALIGA;
	d->offset = 0;

	d->lp = getConfigAsNumber(config, NAME_LP, DEFAULT_LP);
	d->hp = getConfigAsNumber(config, NAME_HP, DEFAULT_HP);

	d->searchsize = 10;
	d->nfiles = 0;
	d->filter = 1;

	d->gpre = getConfigAsNumber(config, NAME_PRE, DEFAULT_PRE);
	d->gpost = getConfigAsNumber(config, NAME_POST, DEFAULT_POST);

	d->prephase = 20;
	d->postphase = 30;
	d->has = 0;
	d->hidephase = 0;
	d->max = 10;
	d->glb = glb;
	d->currentdir = 0;
	strcpy(d->lastaction, "");
	d->overlay = 0;
	d->onlyselected = 0;
	d->insetsearchsize = 2;
	d->plotsearchsize = 0;
	d->zoom = 1.0;
	d->sortmode = 0;

	d->ctl = NULL;
	d->files = NULL;
	d->needsave = 0;

	return d;
}
