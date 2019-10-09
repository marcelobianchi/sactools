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
#include <libgen.h>

/* Some internal definition of methods */
void filtertf(tf * f, defs * d);


/* Picker handling methods */
pdefs *killpicker(pdefs *pick) {
	if (pick == NULL) return NULL;
	
	strcpy(pick->pickLabel, "");
	strcpy(pick->destinationPhase, "");
	strcpy(pick->referencePhase, "");
	
	if (pick->markPhase != NULL) {
		for (;pick->nPhase > 0; pick->nPhase --) {
			free(pick->markPhase[pick->nPhase - 1 ]);
			pick->markPhase[pick->nPhase - 1] = NULL;
		}
		free(pick->markPhase);
		pick->markPhase = NULL;
	}
	
	free(pick);
	return NULL;
}

pdefs *newpicker(PickTypes picktype, char *from, char *to,  char *label) {
	pdefs *pick = NULL;
	SACHEADDEF *def;

	if (label != NULL && strlen(label) > 127)
		return pick;

	if (from == NULL || strlen(from) > 2)
		return pick;

	def = getSacHeadDefFromChar(from);
	if (def == NULL || !def->isMark) return pick;

	if (to == NULL || strlen(to) > 2)
		return pick;

	def = getSacHeadDefFromChar(to);
	if (def == NULL || !def->isMark) return pick;

	pick = malloc(sizeof(pdefs));
	if (pick == NULL) return pick;

	pick->phaseType = picktype;

	if (label != NULL)
		strcpy(pick->pickLabel, label);
	else
		strcpy(pick->pickLabel, "Pick");
	
	strcpy(pick->referencePhase, from);
	strcpy(pick->destinationPhase, to);

	pick->markPhase = NULL;
	pick->nPhase = 0;

	return pick;
}

void pickLoadPhase(pdefs *pick, char *phase) {
	SACHEADDEF *def;

	if (phase == NULL || strlen(phase) > 2)
		return;
	
	def = getSacHeadDefFromChar(phase);
	if (def == NULL || !def->isMark) return;

	pick->nPhase++;
	pick->markPhase = realloc(pick->markPhase, (sizeof(char *) * pick->nPhase));
	pick->markPhase[pick->nPhase - 1] = malloc(sizeof(char) * 3);
	strcpy(pick->markPhase[pick->nPhase - 1], phase);

	return;
}

float pickO(pdefs *pick, SACHEAD *head, int i) {
	float value;
	if (i < 0 || i >= pick->nPhase) return SAC_HEADER_FLOAT_UNDEFINED;
	hdu_getValueFromChar(pick->markPhase[i], head, &value, NULL, NULL);
	return value;
}

char * pickL(pdefs *pick, SACHEAD *head, int i) {
	if (i < 0 || i >= pick->nPhase) return NULL;
	
	char *label;
	char lvar[128];
	sprintf(lvar,"k%s", pick->markPhase[i]);
	hdu_getValueFromChar(lvar, head, NULL, NULL, &label);
	return label;
}

float pickD(pdefs *pick, SACHEAD *head){
	float value;
	hdu_getValueFromChar(pick->destinationPhase, head, &value, NULL, NULL);
	return value;
}

float pickR(pdefs *pick, SACHEAD *head){
	float value;
	hdu_getValueFromChar(pick->referencePhase, head, &value, NULL, NULL);
	return value;
}

void setPick(pdefs *pick, SACHEAD *head, float value) {
	hdu_changeValueFromChar(head, pick->destinationPhase, &value, NULL, NULL);
	
	char lvar[128];
	sprintf(lvar,"k%s", pick->destinationPhase);
	hdu_changeValueFromChar(head, lvar, NULL, NULL, pick->pickLabel);
}

/* Plot help methods */
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
		cpgmtxt("B", k--, where, how, "Update Pick  u");
		cpgmtxt("B", k--, where, how, "Correlate  @");
		cpgmtxt("B", k--, where, how, "Delete d");
		cpgmtxt("B", k--, where, how, "Edit   e");
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
		cpgmtxt("B", k--, where, how, "Nex.Comp >");
		cpgmtxt("B", k--, where, how, "Pre.Comp <");

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

void multitraceplot(defs * d)
{
	int j, k;
	float start;
	char string[1024];
	char stringaux[1024];
	g_ctl **ctl = d->ctl;
	int color = 1;
	char aligChar;
	float x1, x2;
	int lastFile;
	pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
	
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
	if (d->files[d->offset].current) {
		x1 = pickR(pick, d->files[d->offset].current->head) + d->files[d->offset].current->reference - 2.5;
		x2 = pickR(pick, d->files[lastFile].current->head) + d->files[lastFile].current->reference +
				d->postphase;
	} else {
		x1 = 0.0;
		x2 = 10.0;
	}
	
	for (j = 0; j < d->max && (j + d->offset) < d->nfiles; j++) {

		/* *********************************************************** */
		/* Prepare the file to plot                                    */
		/* *********************************************************** */
		tf *thistrace = &d->files[j + d->offset];
		if (thistrace->current == NULL) {
			continue;
		}

	if (d->overlay && thistrace->current->head->f == SAC_HEADER_FLOAT_UNDEFINED) {
			continue;
		}

		if (d->onlyselected && !thistrace->selected)
			continue;

		float Fmark = pickD(pick, thistrace->current->head);
		float Amark = pickR(pick, thistrace->current->head);
		
		/* *********************************************************** */
		/* Switch Aligment mode                                        */
		/* *********************************************************** */

		(j == 0) ? ctl_axisbottom(ctl[j]) : ctl_axisnone(ctl[j]);
		switch (d->alig) {
		case (ALIGO):
			aligChar = 'O';
			if (d->files[0].current->head->o != SAC_HEADER_FLOAT_UNDEFINED) {
				start = thistrace->current->reference - d->files[0].current->head->o;
				ctl_xreset_mm(ctl[j], x1 - d->files[d->offset].current->head->o,
							  x2 - d->files[d->offset].current->head->o);
			} else {
				start = thistrace->current->reference;
				ctl_xreset_mm(ctl[j], x1, x2);
				aligChar = '!';

				SACTIME *time = getTimeStructFromSAC(d->files[0].current->head);
				char *stime = print_time(time, TIME_ISO);
				sprintf(d->lastaction, "Reference is: %s", stime);
				stime = io_freeData(stime);
				time = io_freeData(time);
			}
			break;

		case (ALIGF):
			aligChar = 'F';
			ctl_xreset_mm(ctl[j], -d->prephase, +d->postphase);
			if (Fmark != SAC_HEADER_FLOAT_UNDEFINED) {
				start = thistrace->current->head->b - Fmark;
			} else {
				start = thistrace->current->head->b;
			}
			break;

		case (ALIGA):
			aligChar = 'A';
			ctl_xreset_mm(ctl[j], -d->prephase, +d->postphase);
			if (Amark != SAC_HEADER_FLOAT_UNDEFINED) {
				start = thistrace->current->head->b - Amark;
			} else {
				start = thistrace->current->head->b;
			}
			break;
		}

		/* *********************************************************** */
		/* Plot the trace (DATA)                                       */
		/* *********************************************************** */

		// Selected color
		color = 14;				// Default trace color
		if (thistrace->selected == 1)
			color = 2;			// If event Selected Color = Red
		// if (thistrace->current->head->d>unused25 != 1) color = 14; // If we are in the restricted mode and trace is locked Color = Gray
		if (d->overlay)
			color = j + 1;

		// Finally Plot
		plot(ctl[j],
			 (d->filter
			  && thistrace->current->dataf != NULL) ? thistrace->current->dataf : thistrace->current->data,
			 thistrace->current->head->npts, thistrace->current->head->delta, start, 0, color,
			 d->zoom);
		/* *********************************************************** */

		/* *********************************************************** */
		/* Plot MARKS & Names                                          */
		/* *********************************************************** */
		for(k=0;k<pick->nPhase;k++) {
			if (strcmp(pick->referencePhase, pick->markPhase[k]) == 0 && d->hidephase) continue;

			float value = pickO(pick, thistrace->current->head, k);
			if (value != SAC_HEADER_FLOAT_UNDEFINED) {
				char *label = pickL(pick, thistrace->current->head, k);
				mark(ctl[j], value + start - thistrace->current->head->b, label, 2);
				if (label != NULL) free(label);
				label = NULL;
			}
		}
		
		// IF OVERLAY MODE JUST PLOT THE TRACE AND RETURN.
		if (d->overlay == 1)
			continue;

		// Mark the Window we perform the Min/Max Search in the trace
		if (d->plotsearchsize) {
			mark(ctl[j],
				 Amark + start - thistrace->current->head->b +
				 d->searchsize, "", 3);
			mark(ctl[j],
				 Amark + start - thistrace->current->head->b -
				 d->insetsearchsize, "", 3);
		}

		sprintf(string, "B: %.0f D: %.2f", thistrace->current->head->baz,
				thistrace->current->head->gcarc);
		if (d->putname >= 3)
			cpgmtxt("B", -2.00, 0.0, 0.0, string);
		else
			cpgmtxt("B", -0.70, 0.0, 0.0, string);

		// Add the name of the trace
		if (d->putname > 0) {
			float pos = 1.2;

			cpgsci(2);
			cpgmtxt("R", pos, .5, .5, thistrace->net);
			pos +=  1.0;
			
			if (d->max <= 20) {
				cpgmtxt("R", pos, .5, .5, thistrace->station);
				pos +=  1.0;
			}
			
			if (d->putname >= 2) {
				char *cmp;
				hdu_getValueFromChar("KCMPNM",thistrace->current->head,NULL,NULL, &cmp);
				cpgmtxt("R", pos, .5, .5, cmp);
				cmp = io_freeData(cmp);
				pos +=  1.0;
			}
			
			if (d->putname >= 3) {
				sprintf(string, "%02d] %s", d->offset + j,
						thistrace->current->filename);
				cpgmtxt("B", -0.70, 0.0, 0.0, string);
			}
			
			cpgsci(1);
		}

	}							// End of trace for loop

	// basic information on the foot
	ctl_resizeview(ctl[0]);
	
	// components information
	// green  (should load and loaded)
	// gray   (should not load)
	// red    (should load but not loaded)
	// the < mark the current displayed component
	strcpy(stringaux,"");
	if (d->nfiles > 0) {
		cpgsci((d->files[0].z != NULL)?3:2);
		cpgsch(1.);
		cpgmtxt("B",1.25, -0.05, 1.0, (d->zne == 0)?"Z":"Z");
		
		cpgsci((d->has3 && d->files[0].n != NULL)?3:(d->has3)?2:14);
		cpgmtxt("B",1.25, -0.04, 1.0, (d->zne == 1)?"N":"N");
		
		cpgsci((d->has3 && d->files[0].e != NULL)?3:(d->has3)?2:14);
		cpgmtxt("B",1.25, -0.03, 1.0, (d->zne == 2)?"E":"E");

		cpgsci((d->has3 && d->files[0].r != NULL)?3:(d->has3)?2:14);
		cpgmtxt("B",1.25, -0.02, 1.0, (d->zne == 3)?"R":"R");
		
		cpgsci((d->has3 && d->files[0].t != NULL)?3:(d->has3)?2:14);
		cpgmtxt("B",1.25, -0.01, 1.0, (d->zne == 4)?"T":"T");

		cpgsci(1);

		cpgsch(.65);
		int phaseid = (int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK);
		cpgsci((phaseid == 1)?1:14);
		cpgmtxt("B", 1.25, -0.065, 1.0, PickTypesNames[1]);
		cpgsci((phaseid == 2)?1:14);
		cpgmtxt("B", 2.35, -0.065, 1.0, PickTypesNames[2]);

		cpgsch(1.);
		cpgsci(1);
		cpgmtxt("B", 2.0, -0.05 + d->zne * 0.01, 1.75, "\\m17");

		// Reset to leave
		cpgsch(0.65);
	}
	
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
	} else if (d->has == 1) {
		cpgsci(3);
	} else {
		cpgsci(7);
	}

	cpgmtxt("T", 1.0, 0.5, 0.5, string);

	//plot if the traces are correlated(green) or not (red)
	sprintf(string, "Correlated");
	if (d->files->current->head->unused26 == 1){
		cpgsci(3);
	} else{
		cpgsci(2);
	}

	cpgmtxt("T", 1.0, 0.1, 0.5, string);

	cpgsci(1);

	sprintf(string, "Mag = %.1f", d->files->current->head->mag);
	cpgmtxt("T", 1.0, 0.9, 0.5, string);
}

/* Other io help methods */
void filtertf(tf * f, defs * d)
{
	if (f->current->data == NULL) {
		f->current->dataf = NULL;
		return;
	}

	f->current->dataf = io_freeData(f->current->dataf);

	if (d->filter) {
		yu_rtrend(f->current->data, f->current->head->npts);
		f->current->dataf = iir(f->current->data, f->current->head->npts, f->current->head->delta,
					(d->hp > 0.0) ? 2 : 0, (double)d->hp, (d->lp > 0.0) ? 2 : 0,
					(double)d->lp);

		if (f->current->dataf == NULL) {
			sprintf(message, "Something wrong with the filter %f %f.", d->hp, d->lp);
			alert(WARNING);
		}
	}

	return;
}

void writecurtmp(tf * files, defs * d, float inset, float length, 
				 float maxs, float delta) {
	int j = 0;
	tf *f;
	char cmd[4096];
	char fullpath[2096];
	strcpy(fullpath, "/tmp/kkk.XXXXXX");
	mkdtemp(fullpath);
	
	/*
	 * Write files to temp folder
	 */
	for (j = 0; j < d->nfiles; j++) {
		fullpath[15] = '\0';
		char status[3] = "  ";
		f = &files[j];
		
		char *net = hd_showValueFromChar(f->current->head, "KNETWK", "%s", NULL, NULL);
		char *sta = hd_showValueFromChar(f->current->head, "KSTNM", "%s", NULL, NULL);
		char *evn = hd_showValueFromChar(f->current->head, "KEVNM", "%s", NULL, NULL);
		strcat(fullpath, "/"); strcat(fullpath, net); strcat(fullpath, "."); strcat(fullpath, sta); strcat(fullpath, "."); strcat(fullpath, evn); strcat(fullpath, ".sac");
		free(net); free(sta); free(evn);
		
		if (f->current->head->f == SAC_HEADER_FLOAT_UNDEFINED) {
			strcpy(status, "S-");
			fprintf(stderr, "%s\t[%s]\n", fullpath, status);
			continue;
		}
		status[0] = 'O';
		
		float *ydata = f->current->data;
		status[1] = 'R';
		if (d->filter && f->current->dataf != NULL) {
			status[1] = 'F';
			ydata = f->current->dataf;
		}
		
		fprintf(stderr, "%s\t[%s]\n",fullpath, status);
		io_writeSac(fullpath, f->current->head, ydata);
		
	}
	
	/*
	 * Run SAC Macro
	 */
	fullpath[15] = '\0';
	sprintf(cmd, "cd %s; sac mcpcc_prep.m INSET %f LENGTH %f MAXS %f DELTA %f RUN 1", fullpath, inset, length, maxs, delta);
	system(cmd);
	
	/*
	 * Load Values again
	 */
	for (j = 0; j < d->nfiles; j++) {
		fullpath[15] = '\0';
		f = &files[j];
		
		if (f->current->head->f == SAC_HEADER_FLOAT_UNDEFINED) {
			continue;
		}
		
		char *net = hd_showValueFromChar(f->current->head, "KNETWK", "%s", NULL, NULL);
		char *sta = hd_showValueFromChar(f->current->head, "KSTNM", "%s", NULL, NULL);
		char *evn = hd_showValueFromChar(f->current->head, "KEVNM", "%s", NULL, NULL);
		strcat(fullpath, "/"); strcat(fullpath, net); strcat(fullpath, "."); strcat(fullpath, sta); strcat(fullpath, "."); strcat(fullpath, evn); strcat(fullpath, ".sac");
		free(net); free(sta); free(evn);
		
		SACHEAD *h = io_readSacHead(fullpath);
		if (h == NULL) {
			fprintf(stderr, "Failed to read data back from mcpcc.\n");
			continue;
		}
		// fprintf(stderr, "New time is %f old was %f / %f\n",h->t8, h->f, f->current->head->f);
		f->current->head->f = h->t8;
		free(h);
		h = NULL;
	}
	
	/*
	 * Remove the temporary folder
	 * 
	 * This is UGLY as HELL ! FIX ME !!
	 */
	fullpath[15] = '\0';
	sprintf(cmd, "rm -rf %s", fullpath);
	system(cmd);
	
	d->needsave = 1;
	
	return;
}

void writeout(tf * files, defs * d)
{
	tf *f;
	int j;
	int anyerr = 0;
	
	for (j = 0; j < d->nfiles; j++) {
		f = &files[j];
		if (f != NULL && f->current->head != NULL && f->current->filename != NULL) {
			//Phase P
			if (getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK) == P){
				f->current->head->unused11 = d->lp;
				f->current->head->unused12 = d->hp;
			}
			else { //Phase S
				f->current->head->unused6 = d->lp;
				f->current->head->unused7 = d->hp;
			}

			int err = io_writeSacHead(f->current->filename, f->current->head);
			if (err != 0) {
				anyerr ++;
				fprintf(stderr, "Failed to write file: %s\n", f->current->filename);
			}
		}
	}
	
	if (anyerr > 0) {
		sprintf(message, "Something wrong saving the %d files, please check the terminal window!.", anyerr);
		alert(WARNING);
	}
	
	d->needsave = (anyerr == 0 ) ? 0 : 1;
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

void synch(tf * f, int nfiles)
{
	tf *ref;
	int j;

	//    c<-b2>d------/
	//  a<---b1-->b----/--
	// REF = a to c + b2
	// for the first trace (reference one) is a to a + b1

	// Set the reference
	ref = &f[0];
	if (ref->z != NULL) ref->z->reference = ref->z->head->b;
	if (ref->n != NULL) ref->n->reference = ref->n->head->b;
	if (ref->e != NULL) ref->e->reference = ref->e->head->b;

	for (j = 1; j < nfiles; j++) {
		tf *t = &f[j];
		
		// Z
		if ((ref->z != NULL )&&(t->z != NULL)&&(ref->z->head->nzyear == t->z->head->nzyear)) {
			double val = (t->z->head->nzjday - ref->z->head->nzjday) * 24 * 60 * 60 +
				(t->z->head->nzhour - ref->z->head->nzhour) * 60 * 60 +
				(t->z->head->nzmin  - ref->z->head->nzmin) * 60 +
				(t->z->head->nzsec  - ref->z->head->nzsec) +
				(t->z->head->nzmsec - ref->z->head->nzmsec) / 1000.0 + t->z->head->b;
			t->z->reference = val;
		}
		
		// N
		if ( (ref->n != NULL ) && (t->n != NULL) && (t->n != NULL && ref->n->head->nzyear == t->n->head->nzyear)) {
			double val = (t->n->head->nzjday - ref->n->head->nzjday) * 24 * 60 * 60 +
				(t->n->head->nzhour - ref->n->head->nzhour) * 60 * 60 +
				(t->n->head->nzmin  - ref->n->head->nzmin) * 60 +
				(t->n->head->nzsec  - ref->n->head->nzsec) +
				(t->n->head->nzmsec - ref->n->head->nzmsec) / 1000.0 + t->n->head->b;
			t->n->reference = val;
		}
		
		// E
		if ((ref->e != NULL ) && (t->e != NULL) && (t->e != NULL && ref->e->head->nzyear == t->e->head->nzyear)) {
			double val = (t->e->head->nzjday - ref->e->head->nzjday) * 24 * 60 * 60 +
				(t->e->head->nzhour - ref->e->head->nzhour) * 60 * 60 +
				(t->e->head->nzmin  - ref->e->head->nzmin) * 60 +
				(t->e->head->nzsec  - ref->e->head->nzsec) +
				(t->e->head->nzmsec - ref->e->head->nzmsec) / 1000.0 + t->e->head->b;
			t->e->reference = val;
		}
		// R
		if ( (ref->r != NULL ) && (t->r != NULL) && (t->r != NULL && ref->r->head->nzyear == t->r->head->nzyear)) {
			double val = (t->r->head->nzjday - ref->r->head->nzjday) * 24 * 60 * 60 +
				(t->r->head->nzhour - ref->r->head->nzhour) * 60 * 60 +
				(t->r->head->nzmin  - ref->r->head->nzmin) * 60 +
				(t->r->head->nzsec  - ref->r->head->nzsec) +
				(t->r->head->nzmsec - ref->r->head->nzmsec) / 1000.0 + t->r->head->b;
			t->r->reference = val;
		}
		// T
		if ( (ref->t != NULL ) && (t->t != NULL) && (t->t != NULL && ref->t->head->nzyear == t->t->head->nzyear)) {
			double val = (t->t->head->nzjday - ref->t->head->nzjday) * 24 * 60 * 60 +
				(t->t->head->nzhour - ref->t->head->nzhour) * 60 * 60 +
				(t->t->head->nzmin  - ref->t->head->nzmin) * 60 +
				(t->t->head->nzsec  - ref->t->head->nzsec) +
				(t->t->head->nzmsec - ref->t->head->nzmsec) / 1000.0 + t->t->head->b;
			t->n->reference = val;
		}

	}
}

int sortDist(const void *p1, const void *p2)
{
	tf *t1 = (tf *) p1;
	tf *t2 = (tf *) p2;

	return (t1->z->head->gcarc) > (t2->z->head->gcarc);
}

int sortBaz(const void *p1, const void *p2)
{
	tf *t1 = (tf *) p1;
	tf *t2 = (tf *) p2;

	return (t1->z->head->baz) > (t2->z->head->baz);
}

void checkTREF(tf * files, int nfiles)
{
	int has = 1;
	char phase[4];
	char kphase[10];
	int i;

	for (i = 0; i < nfiles; i++)
		if (files[i].z->head->a == SAC_HEADER_FLOAT_UNDEFINED)
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
		hdu_getValueFromChar(phase, files[i].z->head, &val, NULL, NULL);
		hdu_getValueFromChar(kphase, files[i].z->head, NULL, NULL, &name);

		// Check
		if (val == SAC_HEADER_FLOAT_UNDEFINED) {
			name = io_freeData(name);
			strcpy(message,
				   "The desired header variable is also undefined.");
			alert(WARNING);
			continue;
		}
		// Set
		files[i].z->head->a = val;
		changeCharValueFromChar(files[i].z->head, kphase, name);
		name = io_freeData(name);
	}
}

void *otffree(otf *otf) {
	if (otf != NULL) {
		if (otf->filename != NULL) free(otf->filename);
		otf->filename = NULL;
		otf->data  = io_freeData(otf->data);
		otf->dataf = io_freeData(otf->dataf);
		otf->head  = io_freeData(otf->head);
	}
	free(otf);
	return NULL;
}

void tffree(tf * tf, int n)
{
	int i;

	if (tf == NULL) {
		return;
	}

	for (i = 0; i < n; i++) {
		if (tf[i].station != NULL)  free(tf[i].station);
		tf[i].station = NULL;

		if (tf[i].net != NULL) free(tf[i].net);
		tf[i].net = NULL;

		tf[i].current = NULL;

		tf[i].z = otffree(tf[i].z);
		tf[i].n = otffree(tf[i].n);
		tf[i].e = otffree(tf[i].e);
		tf[i].r = otffree(tf[i].r);
		tf[i].t = otffree(tf[i].t);
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
	d = calloc(1, sizeof(defs));

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

	d->pickRules = NULL;
	d->zne = 0;
	d->has3 = 0;
	d->ctl = NULL;
	d->files = NULL;
	d->needsave = 0;
	d->needfilter = 0;

	return d;
}
