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
#include <timeu.h>
#include <ftw.h>

char version[] = "sachinfo version 0.3.2";

/* Global vars */
char *var[500];
int n = 0;
char *fileend;
int fullprint = 0;
int fulltime = 0;
int noname = 0;

int process(char *filename);
void showHelp();
int preprocess(const char *fpath, const struct stat *sb, int typeflag);
int blacklisted(char *var);
void showVars();

int main(int argc, char **argv)
{
	int i;
	int j;
	char *filename = NULL, *dir = NULL;
	int stop;
	extern char *var[];
	extern int n;
	extern int fullprint;

	/* Files extension used in the search */
	char *defaultfileend = ".sac";
	extern char *fileend;

	if (argc == 1) {
		showHelp();
		return -1;
	}


	/* Inicialization */
	n = 0;
	stop = 0;
	fileend = defaultfileend;
	fullprint = 0;
	fulltime = 0;
	noname = 0;
	for (i = 1; i < argc; i++) {
		if ((j = strncmp(argv[i], "-v", 2)) == 0) {
			fprintf(stderr,
					"Version: %s\n         %s\n         %s\n	 %s\n",
					version, io_version, hd_version, hdu_version);
			return 0;
		}

		if ((j = strncmp(argv[i], "-h", 2)) == 0) {
			showHelp();
			return 0;
		}

		if ((j = strncmp(argv[i], "--fulltime", 10)) == 0) {
			fulltime = 1;
			continue;
		}

		if ((j = strncmp(argv[i], "--nonames", 9)) == 0) {
			noname = 1;
			continue;
		}

		if ((j = strncmp(argv[i], "--vars", 6)) == 0) {
			showVars();
			return 0;
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

		if (dir == NULL && i < argc - 1) {
			filename = argv[i];
			i++;
		}

		while (i < argc) {
			var[n] = argv[i];
			i++;				/* Go to value */
			if ((j = getSacOffsetFromChar(var[n])) == -1) {	/* Check if var exist */
				fprintf(stderr,
						"Variable '%s' doesn't exist, please try again.\n",
						var[n]);
				stop = 1;
			}
			n++;
		}
		continue;
	}

	if (n == 0) {
		fprintf(stderr, "Nothing to do, leaving.\n");
		stop = 1;
	}

	if (stop == 1)
		return -1;

	if (dir != NULL)
		fullprint = 1;
	if (n > 1)
		fullprint = 1;

	if ((filename != NULL) && (dir == NULL))
		process(filename);
	else if ((filename == NULL) && (dir != NULL))
		ftw(dir, preprocess, 20);
	else {
		fprintf(stderr, "Error, no file nor directory to process.\n");
		return -1;
	}

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
	int i;
	SACHEAD *h = NULL;
	char *string = NULL;

	extern char *var[];
	extern int n;
	extern int fullprint;

	h = io_readSacHead(filename);
	if (h != NULL) {
		if (fullprint && noname == 0)
			fprintf(stdout, "%s\t", filename);
	} else {
		if (fullprint)
			fprintf(stdout, "Error reading file (%s)]\n", filename);
		return 1;
	}

	for (i = 0; i < n; i++) {
		string = hd_showValueFromChar(h, var[i], NULL, NULL, NULL);
		if (string == NULL)
			continue;

		SACHEADDEF *sd = getSacHeadDefFromChar(var[i]);
		if (sd->isMark && fulltime) {
			free(string);
			string = NULL;
			string = marktime(h, var[i], TIME_CHGMT);
		}

		if (fullprint)
			fprintf(stdout, "%s\t", string);
		else
			fprintf(stdout, "%s\n", string);
		io_freeData(string);
	}

	if (fullprint)
		fprintf(stdout, "\n");
	io_freeData(h);

	return 0;
}

void showVars()
{
	fprintf(stderr, "\n\n  List of sac Header variables:\n\n");
	showHeadElements(" ");
}

void showHelp()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr,
			"  sachinfo [options] <-d <dir> <var1> ... <varN>|<file> <var1> ... <varN>\n");
	fprintf(stderr, "\n\t-h) Show this help.\n");
	fprintf(stderr, "\t-v) Show program version.\n");
	fprintf(stderr, "\t--nonames) Dont print the filenames, only variables.\n");
	fprintf(stderr, "\t--fulltime) When the indicated variable is a mark, print its fulltime (Y M D H M S) instead of seconds from Reference.\n");
	fprintf(stderr, "\t--vars) Show list of sac header variables.\n");

	fprintf(stderr,
			"\t-e) Supply file ending when searching for files.\n");
	fprintf(stderr, "\t-d) Supply directory for searching for files.\n");
}
