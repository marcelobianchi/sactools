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

#include <head.h>
#include <sacio.h>

#include <ftw.h>

char version[] = "sacrgaps version 0.1";

/* Global vars */
char *fileend;
float gapval = -2147483647;
int method = 1;

int removegaps(SACHEAD * h, float *y, float gapval, int method);
int process(char *filename);
void showHelp();
int preprocess(const char *fpath, const struct stat *sb, int typeflag);
int blacklisted(char *var);

int main(int argc, char **argv)
{
	int i;
	int j;
	char **filename = NULL, *dir = NULL;
	int n;
	int stop;

	/* Files extension used in the search */
	char *defaultfileend = ".sac";
	extern char *fileend;

	/* Mothod of gap removing */
	/* 0 - linear interpolation */
	/* 1 - constant mean */
	extern int method;


	if (argc == 1) {
		showHelp();
		return -1;
	}

	/* Inicialization */
	stop = 0;
	n = 0;
	fileend = defaultfileend;

	for (i = 1; i < argc; i++) {
		if ((j = strncmp(argv[i], "-v", 2)) == 0) {
			fprintf(stderr,
					"Version: %s\n         %s\n         %s)\n",
					version, io_version, hd_version);
			return 0;
		}

		if ((j = strncmp(argv[i], "-h", 2)) == 0) {
			showHelp();
			return 0;
		}

		if ((j = strncmp(argv[i], "-g", 2)) == 0 && i < argc - 1) {
			gapval = atof(argv[i + 1]);
			i++;
			continue;
		}

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

		if ((j = strncmp(argv[i], "-f", 2)) == 0 && i < argc - 1) {
			i++;
			while (i < argc) {
				if (argv[i][0] == '-')
					break;
				filename = realloc(filename, sizeof(char *) * (n + 1));
				filename[n] = argv[i];
				i++;
				n++;
			}
			continue;
		}

		if ((j = strncmp(argv[i], "-m", 2)) == 0 && i < argc - 1) {
			method = atoi(argv[i + 1]);
			if (method < 0 || method > 1) {
				fprintf(stderr, "Invalid method.\n");
				stop = 1;
			}
			i++;
			continue;
		}
	}

	if ((dir == NULL) && (n == 0)) {
		fprintf(stderr, "Nothing to do, leaving.\n");
		stop = 1;
	}

	if (stop == 1)
		return -1;

	if ((n > 0) && (dir == NULL))
		for (i = 0; i < n; i++) {
			process(filename[i]);
	} else if ((n == 0) && (dir != NULL))
		ftw(dir, preprocess, 20);
	else {
		fprintf(stderr,
				"Error, no file nor directory to process or both.\n");
		return -1;
	}

	if (filename != NULL)
		free(filename);
	filename = NULL;

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

int process(char *filename)
{
	SACHEAD *h = NULL;
	float *y = NULL;
	extern float gapval;
	extern int method;
	int i;

	y = io_readSac(filename, &h);

	if (h != NULL && y != NULL) {
		fprintf(stdout, "[%s\t-> ", filename);
	} else {
		fprintf(stdout, "[Erro lendo arquivo (%s)]\n", filename);
		return 1;
	}

	i = removegaps(h, y, gapval, method);
	fprintf(stdout, "%d gap%s ", i, (i == 1) ? "" : "s");

	if (i > 0) {
		io_writeSac(filename, h, y);
		fprintf(stdout, " wr");
	}

	io_freeData(h);
	io_freeData(y);

	fprintf(stdout, "]\n");
	return 0;
}

void showHelp()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr,
			"  sacrgap [-v] [-h] [-g <gap value>] [-m <0|1>] [-e <file ending string>] <-d <dir>|-f <file1> ... <fileN>>\n");
	fprintf(stderr, "\n\tMethod 0: Mean\n");
	fprintf(stderr, "\tMethod 1: Trend (Default)\n");

	fprintf(stderr, "\n\t-h) Show this help.\n");
	fprintf(stderr, "\t-v) Show program version.\n");
	fprintf(stderr, "\t-g) Inform a different gap value (default=%.1f).\n",
			gapval);
	fprintf(stderr,
			"\t-m) Use linear trend or mean value to correct gaps.\n");
	fprintf(stderr,
			"\t-e) Supply file ending when searching for files.\n");
	fprintf(stderr, "\t-d) Supply directory for searching for files.\n");
	fprintf(stderr, "\t-f) Supply filenames for process.\n");


}

int removegaps(SACHEAD * h, float *y, float gapval, int method)
{
	float fillvalue, incvalue;
	int i, j;
	int gapcount;

	gapcount = 0;
	i = 0;
	while (i < h->npts) {
		if (y[i] == gapval) {
			gapcount++;
			j = i;

			while ((y[j] == gapval) && (j < h->npts))
				j++;

			if (i == 0) {
				incvalue = 0;
				fillvalue = y[j];
			} else if ((j == h->npts)) {
				incvalue = 0;
				fillvalue = y[i];
			} else {
				if (method == 1) {
					incvalue = (y[j] - y[i - 1]) / (float) (j - i);
					fillvalue = y[i - 1];
				} else {
					incvalue = 0;
					fillvalue = (y[i - 1] + y[j + 1]) / 2.0;
				}
			}

			for (; i < j; i++) {
				fillvalue += incvalue;
				y[i] = fillvalue;
			}
		}
		i++;
	}

	return gapcount;
}
