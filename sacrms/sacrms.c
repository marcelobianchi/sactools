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
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <sac.h>
#include <head.h>
#include <ftw.h>

#define ABS 0
#define MAX 1
#define MIN 2


char *var[500];
int n = 0;
char *fileend;

int filter = 0;
int NH = 0;
int NL = 0;
float FH, FL;
int rtrend = 0;
char *input = NULL;
float inset;
float onset;

int cc = 0;
long tot = 0;

void showHelp()
{
	fprintf(stdout, "Usage:\n");
	fprintf(stdout,
			"\tsacrms\n\t\t[-h (Help)]\n\t\t[-nd (don't de-mean window)]\n\t\t[-min|-max (default is to mark the global (absolute) max amplitude value)]\n\t\t<-f sac filename>\n\t\t<-i reference field>\n\t\t<-o output mark field>\n\t\t<-w window size to consider from input field>\n");
}

double rms(float *y, SACHEAD * hdr, float b, float e)
{
	double rms = -1.0;

	/* Get samples bounding the window */
	int bn = hdu_getNptsFromSeconds(hdr, b);
	int en = hdu_getNptsFromSeconds(hdr, e);
	int i;

	if (hdu_checkNPTS(hdr, bn) || hdu_checkNPTS(hdr, en)) {
		return rms;
	}

	rms = 0.0;
	for (i = bn; i <= en; i++)
		rms += y[i] * y[i];

	rms = sqrt(rms / (en - bn + 1));

	return rms;
}


int process(char *filename)
{
	float *data = NULL;
	SACHEAD *hdr = NULL;
	int err;
	float picktime;

	if (filename == NULL)
		err = fprintf(stderr, "E:> Invalid filename.\n");
	if (input == NULL)
		err = fprintf(stderr, "E:> Invalid input field.\n");

	if (err != 0) {
		return -1;
	}

	filter = ((NH != 0) || (NL != 0));

	/* data input */
	data = io_readSac(filename, &hdr);
	if (data == NULL) {
		fprintf(stderr, "Error reading file: %s\n", filename);
		return -1;
	}

	/* Filter data if necessary */
	if (filter) {
		yu_rtrend(data, hdr->npts);
		float *temp = iir(data, hdr->npts, hdr->delta, NH, FH, NL, FL);
		if (temp != NULL) {
			io_freeData(data);
			data = temp;
		}
	} else if (rtrend == 1) {
		yu_rtrend(data, hdr->npts);
	}

	/* Check pick variables */
	err = 0;

	if (!isMarkField(input)) {
		fprintf(stderr,
				"Variable indicated for input field is not valid.\n");
		err = 1;
	}

	if (err)
		return -1;

	/* find desired pick time */
	err = hdu_getValueFromChar(input, hdr, &picktime, NULL, NULL);
	if ((err != TYPE_FLOAT) || (picktime == SAC_HEADER_FLOAT_UNDEFINED)) {
		fprintf(stderr,
				"Variable %s is undefined (%f) or other error.\n",
				input, picktime);
		return -1;
	}

	/* get RMS values */
	double rms_before, rms_after, rms_hit;
	int inset = 5.0;
	int onset = 15;

	char class[2] = { 'C', '\0' };
	rms_before =
		rms(data, hdr, picktime - inset - 25.0, picktime - inset - 5.0);
	rms_hit = rms(data, hdr, picktime - inset, picktime + onset);

	if (rms_before * 2.2 < rms_hit) {
		class[0] = 'B';

		rms_hit = rms(data, hdr, picktime - inset, picktime + onset / 2);
		rms_after =
			rms(data, hdr, picktime + onset / 2,
				picktime + onset / 2 + (onset + inset));

		if (rms_hit > rms_after)
			class[0] = 'A';
	}

	/* report */
	if (cc == 99) {
		fprintf(stderr, ". [%04ld]\n\t", ++tot);
		cc = 0;
	} else {
		fprintf(stderr, ".");
		cc++;
		tot++;
	}

	/* fprintf(stderr,"RMS: %s  %f %f %f => %s\n",filename, rms_before, rms_hit, rms_after,class); */

	changeCharValueFromChar(hdr, "kuser0", class);
	io_writeSacHead(filename, hdr);

	/* quit */
	io_freeData(data);
	io_freeData(hdr);
	return 0;
}


int preprocess(const char *fpath, const struct stat *sb, int typeflag)
{
	int i = 0;
	char *path = NULL;
	extern char *fileend;

	if ((typeflag == FTW_F) &&
		((i =
		  strncasecmp(&fpath[strlen(fpath) - strlen(fileend)], fileend,
					  strlen(fileend))) == 0)
		) {
		path = malloc((strlen(fpath) + 1) * sizeof(char));
		if (path != NULL) {
			strcpy(path, fpath);
			i = process(path);
			if (path != NULL)
				free(path);
			path = NULL;
		} else {
			i = 1;
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	/* Files extension used in the search */
	char *filename = NULL, *dir = NULL;


	int i, j;
	char *defaultfileend = ".sac";
	extern char *fileend;
	int stop;

	if (argc == 1) {
		showHelp();
		return 0;
	}

	n = 0;
	stop = 0;
	fileend = defaultfileend;

	/* check of parameters */
	for (i = 1; i < argc; i++) {
		if ((j = strncmp(argv[i], "-d", 2)) == 0 && i < argc - 1) {
			dir = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-e", 2)) == 0 && i < argc - 1) {
			fileend = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-hp", 3)) == 0 && i < argc - 1) {
			NH = 2;
			FH = atof(argv[i + 1]);
			i++;
			fprintf(stderr, "I:> High Pass On: %.2f Hz\n", FH);
			continue;
		}

		if ((j = strncmp(argv[i], "-lp", 3)) == 0 && i < argc - 1) {
			NL = 2;
			FL = atof(argv[i + 1]);
			i++;
			fprintf(stderr, "I:> Low Pass On: %.2f Hz\n", FL);
			continue;
		}

		if ((j = strncmp(argv[i], "-h", 2)) == 0) {
			showHelp();
			return 0;
		}

		if ((j = strncmp(argv[i], "-mark", 5)) == 0 && i < argc - 1) {
			input = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-rtrend", 7)) == 0) {
			rtrend = 1;
			fprintf(stderr, "I:> Removing the trend.\n");
			continue;
		}


		if ((j = strncmp(argv[i], "-inset", 6)) == 0 && i < argc - 1) {
			inset = atof(argv[i + 1]);
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-onset", 6)) == 0 && i < argc - 1) {
			onset = atof(argv[i + 1]);
			i++;
			continue;
		}

		if (dir == NULL && filename == NULL && i < argc - 1) {
			filename = argv[i];
			fprintf(stderr, "I:> Using filename: %s\n", filename);
			i++;
		} else {
			fprintf(stderr,
					"E:> You already specified a dir, cannot take a file.\n");
			stop = 1;
		}

		continue;
	}

	if (input == NULL)
		stop = fprintf(stderr, "E:> No Input Mark.\n");

	if (stop != 0) {
		return -1;
	}

	if ((filename != NULL) && (dir == NULL))
		process(filename);
	else if ((filename == NULL) && (dir != NULL)) {
		cc = tot = 0;
		fprintf(stderr, "I:> Processing: \n\t");
		ftw(dir, preprocess, 20);
		fprintf(stderr, "[%04ld]\n", tot);
	} else {
		fprintf(stderr, "Error, no file nor directory to process.\n");
		return -1;
	}
	return 0;
}
