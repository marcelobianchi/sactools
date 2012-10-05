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
#include <string.h>

#include <inspect.h>

#include <io.h>
#include <aux.h>
#include <edit_tf.h>
#include <globers.h>

/* Config for plotting the config screen */
float col1 = 0.35;
float col2 = 0.37;
float col3 = 0.90;
float col4 = 0.95;
float spacing = 1.1;

void PK_checkoption(defs * d, char ch, float ax, float ay);

int booloption(int value, int pos, char *c, char *message)
{
	float line = pos * spacing;

	cpgmtxt("T", line, col4, 1.0, c);
	cpgmtxt("T", line, col1, 1.0, message);

	if (value == 1)
		cpgsci(3);
	else
		cpgsci(14);
	cpgmtxt("T", line, col2, 0.0, "Enabled");

	if (value == 0)
		cpgsci(2);
	else
		cpgsci(14);
	cpgmtxt("T", line, col3, 0.0, "Disabled");

	cpgsci(1);
	return (--pos);
}

int multoption(int value, int pos, char *c, char *message, char **option,
			   int no)
{
	float line;
	int i;

	line = pos * spacing;

	cpgmtxt("T", line, col4, 1.0, c);
	cpgmtxt("T", line, col1, 1.0, message);

	for (i = 0; i < no; i++) {
		if (value == i)
			cpgsci(3);
		else
			cpgsci(14);
		cpgmtxt("T", line, col2, 0.0, option[i]);
		pos--;
		line = pos * spacing;
	}

	cpgsci(1);
	return --pos;
}

int valueoptionchar(char *value, int pos, char *c, char *message,
					char *defaultv)
{
	float line = pos * spacing;
	char cvalue[200];

	cpgmtxt("T", line, col1, 1.0, message);

	if (strlen(value) == strlen(defaultv)
		&& strncmp(value, defaultv, strlen(value)) == 0)
		cpgsci(3);
	else
		cpgsci(2);
	cpgmtxt("T", line, col2, 0.0, value);

	cpgsci(14);
	cpgmtxt("T", line, col3, 1.0, defaultv);

	cpgsci(1);
	cpgmtxt("T", line, col4, 1.0, c);

	return --pos;
}

int valueoption(float value, int pos, char *c, char *message,
				float defaultv)
{
	char cvalue[200], cdefaultv[200];
	sprintf(cvalue, "%.4f", value);
	sprintf(cdefaultv, "%.4f", defaultv);
	return valueoptionchar(cvalue, pos, c, message, cdefaultv);
}

int valueoptionbool(int value, int pos, char *c, char * message,
				int defaultv) {
	char cvalue[200], cdefaultv[200];
	sprintf(cvalue, "%s", (value)?"true":"false");
	sprintf(cdefaultv, "%s", (defaultv)?"true":"false");
	return valueoptionchar(cvalue, pos, c, message, cdefaultv);
}

int titleoption(char *message, int pos)
{
	float line;

	pos--;
	line = pos * spacing;
	cpgsci(13);
	cpgmtxt("T", line, 0.5, 0.5, message);
	cpgsci(1);
	pos--;

	return --pos;
}

int subtitleoption(char *message, int pos)
{
	float line;

	pos--;
	line = pos * spacing * 0.7;
	cpgsci(11);
	cpgmtxt("T", line, 0.5, 0.5, message);
	cpgsci(1);
	pos--;

	return --pos;
}

void Config(defs * d)
{
	g_ctl *Wd = NULL;
	char ch = 'c';
	float ax, ay;
	int k = -1;
	float value;
	char aux[200];

	int reloadTraces = 0;

	Wd = ctl_newctl(0.2, 0.05, .6, .9);
	Wd->expand = 0;

	while (ch != 'Q' && ch != 'q') {
		k = -1;
		ctl_resizeview(Wd);
		ctl_clean(NULL);
		k = titleoption("Configuration:", k);
		k = subtitleoption("config file is ~/.sacinspectrc", k--);

		k = valueoption(getConfigAsNumber(config, NAME_LP, DEFAULT_LP), k,
						"l", "Default Low Pass filter for new events:",
						DEFAULT_LP);
		k = valueoption(getConfigAsNumber(config, NAME_HP, DEFAULT_HP), k,
						"h", "Default High Pass filter for new events:",
						DEFAULT_HP);
		k = valueoption(getConfigAsNumber(config, NAME_PRE, DEFAULT_PRE),
						k, "a", "Show Window in global align mode (pre):",
						DEFAULT_PRE);
		k = valueoption(getConfigAsNumber(config, NAME_POST, DEFAULT_POST),
						k, "s",
						"Show Window in global align model (post):",
						DEFAULT_POST);
		k = valueoption(getConfigAsNumber
						(config, NAME__ZOOMGAIN, DEFAULT_ZOOMGAIN), k, "z",
						"Gain multiplier when zoom is active:",
						DEFAULT_ZOOMGAIN);

		// Jump a line
		k--;

		k = valueoptionchar(getConfigAsString
							(config, NAME_PATTERN, DEFAULT_PATTERN), k,
							"f",
							"Pattern used to search for folders (Need re-start):",
							DEFAULT_PATTERN);
		k = valueoptionchar(getConfigAsString
							(config, NAME_Z, DEFAULT_Z), k,
							"1",
							"Pattern used to search for files for the Z component:",
							DEFAULT_Z);

		k = valueoptionchar(getConfigAsString
							(config, NAME_N, DEFAULT_N), k,
							"2",
							"Pattern used to search for files for the N component:",
							DEFAULT_N);

		k = valueoptionchar(getConfigAsString
							(config, NAME_E, DEFAULT_E), k,
							"3",
							"Pattern used to search for files for the E component:",
							DEFAULT_E);

		// Jump a line
		k--;

		k = valueoptionbool(getConfigAsBoolean
							(config, NAME_LOAD, DEFAULT_LOAD), k,
							"c",
							"Load All Components:",
							DEFAULT_LOAD);

		// Jump a line
		k--;

		k = valueoptionchar(PickTypesNames[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)],
							k,
							"m",
							"Pick Mode:",
							PickTypesNames[DEFAULT_PICK]);

		k -= 12;
		k = titleoption("Q - To Return !", k);

		ch = getonechar(&ax, &ay);
		switch (ch) {
		case ('z'): {
			value = lerfloat("Enter new zoom gain?");
			setConfigNumber(config, NAME__ZOOMGAIN, value);
			break;
		}
			
		case ('l'): {
			value = lerfloat("Low pass filter Hz?");
			setConfigNumber(config, NAME_LP, value);
			break;
		}
			
		case ('h'): {
			value = lerfloat("High pass filter Hz?");
			setConfigNumber(config, NAME_HP, value);
			break;
		}
			
		case ('a'): {
			value =
					fabs(lerfloat
						 ("Time in seconds before the theoretical mark?"));
			setConfigNumber(config, NAME_PRE, value);
			break;
		}
			
		case ('s'): {
			value =
					fabs(lerfloat
						 ("Time in seconds after the theoretical mark?"));
			setConfigNumber(config, NAME_POST, value);
			break;
		}
			
		case ('f'): {
			lerchar("Enter a new folder pattern?", aux, 200);
			setConfigString(config, NAME_PATTERN, (strlen(aux)>0)?aux:DEFAULT_PATTERN);
			break;
		}
			
		case ('1'): {
			lerchar("Enter a new file pattern?", aux, 200);
			setConfigString(config, NAME_Z, (strlen(aux)>0)?aux:DEFAULT_Z);
			reloadTraces = 1;
			break;
		}
			
		case ('2'): {
			lerchar("Enter a new file pattern?", aux, 200);
			setConfigString(config, NAME_N, (strlen(aux)>0)?aux:DEFAULT_N);
			reloadTraces = 1;
			break;
		}
			
		case ('3'): {
			lerchar("Enter a new file pattern?", aux, 200);
			setConfigString(config, NAME_E, (strlen(aux)>0)?aux:DEFAULT_E);
			reloadTraces = 1;
			break;
		}
			
		case ('c'): {
			value = (getConfigAsBoolean(config, NAME_LOAD, DEFAULT_LOAD)) ? 0 : 1 ;
			setConfigBoolean(config, NAME_LOAD, value);
			reloadTraces = 1;
			break;
		}
			
		case ('m'): {
			if (getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK) == P) {
				setConfigNumber(config, NAME_PICK, S);
			} else {
				setConfigNumber(config, NAME_PICK, P);
			}
			break;
		}
			
		case ('q'):
		case ('Q'): {
			ax = getConfigAsNumber(config, NAME_LP, DEFAULT_LP);
			ay = getConfigAsNumber(config, NAME_HP, DEFAULT_HP);
			if (ax <= ay) {
				ch = 'E';
				strcpy(message,
					   "Low Pass should be larger than high pass.");
				alert(WARNING);
			}
			break;
		}
		}
	}
	
	if (reloadTraces) {
		io_loadEv(d);
		d->needsave = 0;
		sprintf(d->lastaction, "Data Reloaded.");
	}
}

void PK_checkoption(defs * d, char ch, float ax, float ay)
{
	int j, i;
	float vmin, vmax, aux;

	switch (ch) {

	case ('0'): {
		d->sortmode++;
		if (d->sortmode > 1)
			d->sortmode = 0;
		if (d->sortmode == 0) {
			qsort(d->files, d->nfiles, sizeof(tf), sortDist);
			sprintf(d->lastaction, "Sort mode Distance.");
		} else {
			qsort(d->files, d->nfiles, sizeof(tf), sortBaz);
			sprintf(d->lastaction, "Sort mode Back-Azimuth.");
		}
		break;
	}

	case ('!'): {
		if (d->zoom == 1.0) {
			d->zoom =
				getConfigAsNumber(config, NAME__ZOOMGAIN,
								  DEFAULT_ZOOMGAIN);
			if (d->zoom < 0.01) {
				d->zoom = 0.001;
			}
			sprintf(d->lastaction, "Amplitude Zoom On (Gain = %.2f).",
					d->zoom);
		} else {
			d->zoom = 1.0;
			sprintf(d->lastaction, "Amplitude Zoom Off.");
		}
		break;
	}

	case ('*'): {
		if (d->prephase > 3000) {
			sprintf(d->lastaction, "Cannot make it larger.");
			break;
		}
		d->prephase *= 2;
		d->postphase *= 2;
		break;
	}

	case ('/'): {
		if (d->prephase < 2) {
			sprintf(d->lastaction, "Cannot make it smaller.");
			break;
		}
		d->prephase /= 2;
		d->postphase /= 2;
		break;
	}

	case ('r'): {
			if (d->needsave == 1) {
				if (yesno("Picks not saved, save?") == 1)
					writeout(d->files, d);
			}

			io_loadEv(d);
			d->needsave = 0;
		sprintf(d->lastaction, "Data Reloaded.");
		break;
	}

	case (' '): {
		d->onlyselected = !d->onlyselected;
		sprintf(d->lastaction,
				"Activate/De-Activated only selected mode.");
		break;
	}

	case (':'): {
			char station[15];
			lerchar
				("Enter e/<Event name> for search for an event or <NAME> for hightlight a station by name",
				 station, 25);

			if (station[1] == '/') {
				if (station[0] == 'e') {
					if (d->needsave == 1) {
						if (yesno("Picks not saved, save?") == 1)
							writeout(d->files, d);
					}

					for (j = 0; j < d->glb->gl_pathc; j++) {
						if ((i =
							 strncmp(d->glb->gl_pathv[j], &station[2],
									 strlen(&station[2]))) == 0) {
							d->currentdir = j;
							io_loadEv(d);
							d->needsave = 0;
						}
					}
				}
			} else {
				for (j = 0; j < d->nfiles; j++) {
					if (strlen(station) == 0)
						d->files[j].selected = 0;
					else if ((i =
							  strncasecmp(d->files[j].station, station,
										  strlen(station))) == 0) {
						d->files[j].selected = 1;
						fprintf(stderr, "Hitted !");
					}
				}
			}
		sprintf(d->lastaction, "Searched complete.");
		break;
	}

	case ('+'): {
		killCTL(&d->ctl, d->max);
		d->overlay = !d->overlay;
		d->ctl = initCTL(d);
		sprintf(d->lastaction, "Activate/De-Activated overlay mode.");
		break;
	}

	case ('#'): {
		d->hidephase = !d->hidephase;
		sprintf(d->lastaction, "Hide/Un-Hide theoretical phase.");
		break;
	}

	case ('-'): {
		if (d->currentdir == d->glb->gl_pathc - 1)
			break;
		
		if (d->needsave == 1) {
			if (yesno("Picks not saved, save?") == 1)
				writeout(d->files, d);
		}
		
		d->currentdir++;
		while ((d->currentdir) < (d->glb->gl_pathc - 1)) {
			glob_t *glb;
			glb = filelist(d->glb->gl_pathv[d->currentdir], getConfigAsString(config, NAME_Z, DEFAULT_Z));
			if (findHas(glb) == 0) {
				glb = killGlob(glb);
				break;
			}
			glb = killGlob(glb);
			d->currentdir++;
		}
		
		io_loadEv(d);
		d->needsave = 0;
		sprintf(d->lastaction, "Switch to next un-readed data.");
		break;
	}

	case ('n'): {
		d->offset += d->max;
		sprintf(d->lastaction, "Advanced display by %d files", d->max);
		if (d->offset >= d->nfiles) {
			d->offset -= d->max;
			sprintf(d->lastaction, "No more files to advance");
		}
		break;
	}

	case ('p'): {
		d->offset -= d->max;
		sprintf(d->lastaction, "Showing %d previous files", d->max);
		if (d->offset < 0) {
			d->offset = 0;
			sprintf(d->lastaction, "We are already at the first file.");
		}
		break;
	}

	case ('N'): {
			d->currentdir++;
			sprintf(d->lastaction, "Switched to next event (directory).");

			if (d->currentdir >= d->glb->gl_pathc) {
				d->currentdir = d->glb->gl_pathc - 1;
				sprintf(d->lastaction, "This is the last event.");
				break;
			}

			if (d->needsave == 1) {
				if (yesno("Picks not saved, save?") == 1)
					writeout(d->files, d);
			}

			io_loadEv(d);
			d->needsave = 0;
		break;
	}

	case ('P'): {
			d->currentdir--;
			sprintf(d->lastaction,
					"Switched to previous event (directory).");
			if (d->currentdir < 0) {
				d->currentdir = 0;
				sprintf(d->lastaction, "This is the first event.");
				break;
			}

			if (d->needsave == 1) {
				if (yesno("Picks not saved, save?") == 1)
					writeout(d->files, d);
			}

			io_loadEv(d);
			d->needsave = 0;
		break;
	}

	case ('?'): {
		if (d->needsave == 1) {
			if (yesno("Picks not saved, save?") == 1)
				writeout(d->files, d);
		}
		Config(d);
		sprintf(d->lastaction, "Reconfiguration complete.");
		break;
	}

	case ('e'): {
		sprintf(d->lastaction, "Sorry, no trace found on this position.");
		for (j = 0; j < d->max && (j + d->offset < d->nfiles); j++) {
			if (ctl_checkhit(d->ctl[j], ax, ay)) {
				//if (d->files[d->offset + j].h->unused25 != 1 ) continue;

				g_ctl *fullscreen = ctl_newctl(0.08, 0.15, 0.88, 0.7);
				fullscreen->expand = 1;
				ctl_axisfull(fullscreen);

				tf *this = &d->files[d->offset + j];
				edit_tf(fullscreen, this, d);

				free(fullscreen);
				fullscreen = NULL;
				sprintf(d->lastaction, "Edited file %d.", j);
				break;
			}
		}
		break;
	}

	case ('.'): {
		if (d->alig == ALIGF) {
			sprintf(d->lastaction,
					"Change aligment to phase A (theoretical).");
			d->alig = ALIGA;
		} else if (d->alig == ALIGA) {
			sprintf(d->lastaction,
					"Change aligment to Origin Time (Origin Time = 0).");
			d->alig = ALIGO;
		} else if (d->alig == ALIGO) {
			sprintf(d->lastaction, "Change aligment to phase F (pick).");
			d->alig = ALIGF;
		}
		break;
	}

	case ('m'): {
		d->plotsearchsize = !d->plotsearchsize;
		break;
	}

	case ('s'): {
		d->searchsize =
			fabs(lerfloat
				 ("How many seconds post A mark to considerer in Min-Max search ?"));
		break;
	}

	case ('i'): {
		d->insetsearchsize =
			fabs(lerfloat
				 ("How many seconds pre A mark to considerer in Min-Max search ?"));
		break;
	}

	case ('t'): {
		d->putname++;
		if (d->putname == 4)
			d->putname = 0;
		sprintf(d->lastaction,
				"Switch display of filename and station information.");
		break;
	}

	case (','): {
		killCTL(&d->ctl, d->max);
		d->max = lerint("How many traces (0 for all)?");
		if (d->max == 0)
			d->max = d->nfiles;
		d->ctl = initCTL(d);
		sprintf(d->lastaction, "Changed to %d traces per screen.", d->max);
		break;
	}

	case ('o'): {
		cpgopen("plot.ps/CPS");
		cpgsch(0.65);
		cpgslw(2);
		d->printout = 1;
		multitraceplot(d);
		d->printout = 0;
		cpgclos();
		cpgslct(d->grID);
		sprintf(d->lastaction, "Saved plot-screen in file plot.ps");
		break;
	}

	case ('d'): {
		sprintf(d->lastaction, "Sorry, no trace found on this position.");
		for (j = 0; j < d->max && (j + d->offset < d->nfiles); j++) {
			if (ctl_checkhit(d->ctl[j], ax, ay)) {
				setPick(d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)], d->files[d->offset + j].current->head, SAC_HEADER_FLOAT_UNDEFINED);
				sprintf(d->lastaction, "Deleted mark F on trace %d", j);
				d->needsave = 1;
				break;
			}
		}
		break;
	}

	case ('z'): // german changes y and z
	case ('y'): {
		pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
		for (j = d->offset; (j < d->nfiles) && (j < (d->offset + d->max));
			 j++) {
			tf *ts = &d->files[j];
			float AMark = pickR(pick, ts->current->head);
			getminmax((d->filter
					   && ts->current->dataf != NULL) ? ts->current->dataf : ts->current->data, ts->current->head,
					  AMark - d->insetsearchsize,
					  AMark + d->searchsize, &vmin, &vmax);
			// ts->current->head->f = vmax;
			setPick(pick, ts->current->head, vmax);
		}
		d->needsave = 1;
		sprintf(d->lastaction,
				"Searched for maximun between A %.2f adn %.2f seconds.",
				d->insetsearchsize, d->searchsize);
		break;
	}

	case ('c'): {
		pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
		for (j = d->offset; (j < d->nfiles) && (j < (d->offset + d->max));
			 j++) {
			tf *ts = &d->files[j];
			setPick(pick, ts->current->head, SAC_HEADER_FLOAT_UNDEFINED);
		}
		d->needsave = 1;
		sprintf(d->lastaction, "Cleaned pick phases.");
		break;
	}

	case ('x'): {
		pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
		for (j = d->offset; (j < d->nfiles) && (j < (d->offset + d->max));
			 j++) {
			tf *ts = &d->files[j];
			//      if (ts->current->head->unused25 != 1 ) continue;
			float AMark = pickR(pick, ts->current->head);
			getminmax((d->filter
					   && ts->current->dataf != NULL) ? ts->current->dataf : ts->current->data, ts->current->head,
					  AMark - d->insetsearchsize,
					  AMark + d->searchsize, &vmin, &vmax);
			setPick(pick, ts->current->head, vmin);
		}
		d->needsave = 1;
		sprintf(d->lastaction,
				"Searched for minimun between A %.2f adn %.2f seconds.",
				d->insetsearchsize, d->searchsize);
		break;
	}

	case ('1'): {
		pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
		for (j = 0; j < d->nfiles; j++) {
			tf *ts = &d->files[j];
			//      if (ts->current->head->unused25 != 1 ) continue;
			float AMark = pickR(pick, ts->current->head);
			getminmax((d->filter
					   && ts->current->dataf != NULL) ? ts->current->dataf : ts->current->data, ts->current->head,
					  AMark - d->insetsearchsize,
					  AMark + d->searchsize, &vmin, &vmax);
			setPick(pick, ts->current->head, vmax);
		}
		d->needsave = 1;
		sprintf(d->lastaction,
				"Searched for maximun for ALL betwen A %.2f adn %.2f seconds.",
				d->insetsearchsize, d->searchsize);
		break;
	}

	case ('2'): {
		pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
		for (j = 0; j < d->nfiles; j++) {
			tf *ts = &d->files[j];
			// if (ts->current->head->unused25 != 1 ) continue;
			float AMark = pickR(pick, ts->current->head);
			getminmax((d->filter
					   && ts->current->dataf != NULL) ? ts->current->data : ts->current->data, ts->current->head,
					  AMark - d->insetsearchsize,
					  AMark + d->searchsize, &vmin, &vmax);
			setPick(pick, ts->current->head, vmin);
		}
		d->needsave = 1;
		sprintf(d->lastaction,
				"Searched for minimun for ALL betwen A %.2f adn %.2f seconds.",
				d->insetsearchsize, d->searchsize);
		break;
	}

	case ('3'): {
		pdefs *pick = d->pickRules[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)];
		for (j = 0; j < d->nfiles; j++) {
			tf *ts = &d->files[j];
			// if (ts->current->head->unused25 != 1 ) continue;
			setPick(pick, ts->current->head, SAC_HEADER_FLOAT_UNDEFINED);
		}
		d->needsave = 1;
		sprintf(d->lastaction, "Cleaned ALL pick phases.");
		break;
	}

	case ('w'): {
		writeout(d->files, d);
		sprintf(d->lastaction, "Files were writen to disk.");
		break;
	}

	case ('f'): {
		d->filter = !d->filter;
		d->needfilter = 1;
		sprintf(d->lastaction, "Switch filter %s",
				(d->filter) ? "ON" : "OFF");
		break;
	}

	case ('l'): {
		aux = lerfloat("Low pass filter?");
		if ((aux >= d->hp) && (aux >= 0)) {
			d->lp = aux;
			sprintf(d->lastaction, "Low pass filter set to %.4f Hz",
					d->lp);
			d->needfilter = 1;
		} else {
			sprintf(d->lastaction,
					"Invalid low pass filter %.4f Hz. Cannot be smaller than HP.",
					aux);
		}
		break;
	}

	case ('h'): {
		aux = lerfloat("High pass filter?");
		if ((aux <= d->lp) && (aux >= 0)) {
			d->hp = aux;
			sprintf(d->lastaction, "High pass filter set to %.4f Hz",
					d->hp);
			d->needfilter = 1;
		} else {
			sprintf(d->lastaction,
					"Invalid high pass filter %.4f Hz. Cannot be larger than LP.",
					aux);
		}
		break;
	}

	case ('Q'): {
		if (d->needsave == 1) {
			if (yesno("Picks not saved, save?") == 1)
				writeout(d->files, d);
		}
		break;
	}

	case ('<'):
	case ('>'): {
		int currentid = d->zne;
		
		if (!getConfigAsBoolean(config, NAME_LOAD, DEFAULT_LOAD)) {
			sprintf(d->lastaction, "Only Z component loaded. Visit config for activate this option.");
			break;
		}
		if (ch == '>')
			d->zne++;
		else
			d->zne--;
		
		if (d->zne >= 3) d->zne = 0;
		if (d->zne < 0) d->zne = 2;
		if (io_AdjustCurrent(d) == 0) {
			sprintf(d->lastaction, "Switched components to %s", (d->zne == 0)?"Z":(d->zne == 1)?"N":"E");
		} else {
			sprintf(d->lastaction, "Failed to switch, try reloading the event.");
			d->zne = currentid;
		}
		break;
	}

	default: {
		sprintf(d->lastaction, "Cannot understand %c", ch);
		break;
	}
	}
}

char PK_waitloop(defs * d)
{
	float ax = 0.0, ay = 0.0;
	char ch;
	g_ctl *ctlpick = NULL;

	ctlpick = ctl_newctl(0.0, 0.0, 1.0, 1.0);
	ctlpick->expand = 0;
	ctl_resizeview(ctlpick);
	ch = getonechar(&ax, &ay);
	free(ctlpick);
	ctlpick = NULL;
	PK_checkoption(d, ch, ax, ay);

	return ch;
}

/* Main Module method */
void PK_process(glob_t * glb)
{
	char ch = '\0';
	defs *d;

	d = newdefs(glb);

	/* Initialize PGplot subsystem */
	d->grID = opengr();
	cpgsch(0.65);

	/* Load first dir data */
	//	glob_t *glbs = saclist(d);
	//	d->needsave = 0;
	//	d->files = inputme(glbs->gl_pathc, glbs->gl_pathv, d);
	//	glbs = killGlob(glbs);
	io_loadEv(d);
	
	/* Get new CTLs for use */
	d->ctl = initCTL(d);

	/* initialize the pick structures */
	pdefs **picks;
	picks = malloc(sizeof(pdefs*) * 3);
	picks[None] = newpicker(None, "", "", PickTypesNames[None]);
	picks[P]    = newpicker(P, "A", "F", PickTypesNames[P]);
	picks[S]    = newpicker(S, "T0", "T1", PickTypesNames[S]);

	/* Attach */
	if (getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK) > 2) {
		setConfigNumber(config, NAME_PICK, DEFAULT_PICK);
		fprintf(stderr, "Invalid pick phase %f -- Setting default value of %s\n",
				getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK),
				PickTypesNames[(int)getConfigAsNumber(config, NAME_PICK, DEFAULT_PICK)]);
	}
	d->pickRules = picks;
	picks = NULL; 
	
	/* Main Loop */
	while (ch != 'Q') {
		/* Plot Data */
		multitraceplot(d);

		/* Wait for command */
		ch = PK_waitloop(d);
	}

	tffree(d->files, d->nfiles);
	killCTL(&d->ctl, d->max);

	d->pickRules[0] = killpicker(d->pickRules[0]);
	d->pickRules[1] = killpicker(d->pickRules[1]);
	d->pickRules[2] = killpicker(d->pickRules[2]);
	free(d->pickRules);
	d->pickRules = NULL;

	if (d != NULL) free(d);
	d = NULL;

	cpgclos();

	return;
}

glob_t *collector(defs * d, char *net, char *station)
{
	glob_t *glb = NULL;
	char *filepath = NULL;
	char *path;
	int i;

	glb = malloc(sizeof(glob_t));
	glb->gl_pathc = 0;
	glb->gl_pathv = NULL;

	for (i = 0; i < d->glb->gl_pathc; i++) {
		filepath = malloc(sizeof(char) * (strlen(path) + 500));
		path = d->glb->gl_pathv[i];
		sprintf(filepath, "%s/*%s*%s*SAC", path, net, station);
		fprintf(stderr, "Searchin in: %s\n", filepath);
		glob(filepath, GLOB_APPEND, NULL, glb);
		free(filepath);
		filepath = NULL;
	}

	d->has = 0;
	return glb;
}
