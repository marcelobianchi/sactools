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
#include <libgen.h>

/* File tree walk */
#include <ftw.h>

#include <sacio.h>
#include <head.h>

#include <filelist.h>
#include <diskwork.h>

#include <unistd.h>

void showHelp();
void showVersion();
void dumpError(char *string);

/* Global variables */
char version[] = "sacrename version 0.5";
char *defaultdir = NULL;

int main(int argc, char **argv)
{
	static char defaultfileend[5] = ".sac";
	int i, j, k, count, copy;
	char *dir = NULL;
	int realgo, stop, showme;

	if (argc == 1) {
		showHelp();
		return -1;
	}

	/* Inicialization */
	fileend = defaultfileend;
	copy = 0;
	stop = 0;
	showme = 0;
	realgo = 0;

	/* Parsing arguments */
	for (i = 1; i < argc; i++) {
		if ((j = strcasecmp(argv[i], "-v")) == 0) {
			showVersion();
			return 0;
		}

		if ((j = strcasecmp(argv[i], "-h")) == 0) {
			showHelp();
			return 0;
		}

		if ((j = strcasecmp(argv[i], "--luke")) == 0) {
			realgo = 1;
			continue;
		}

		if ((j = strcasecmp(argv[i], "--showme")) == 0) {
			showme = 1;
			continue;
		}

		if ((j = strcasecmp(argv[i], "-c")) == 0) {
			copy = 1;
			continue;
		}

		if ((j = strcasecmp(argv[i], "-m")) == 0) {
			if (i + 1 >= argc)
				dumpError("Not enough parameters to proceed.\n");
			if (argv[i + 1][0] != '/')
				dumpError("Directory name should be an absolute path.\n");
			defaultdir = argv[i + 1];
			i++;
			copy = 1;
			continue;
		}

		if ((j = strcasecmp(argv[i], "-e")) == 0) {
			if (i + 1 >= argc)
				dumpError("Not enough parameters to proceed.\n");
			fileend = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strcasecmp(argv[i], "-d")) == 0) {
			if (nfilelist != 0)
				dumpError("You should use -d or -f\n");
			if (i + 1 >= argc)
				dumpError("Not enough parameters to proceed.\n");
			if (argv[i + 1][0] == '-')
				dumpError("Directory name should not begin with '-'.\n");
			dir = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strcasecmp(argv[i], "-f")) == 0) {
			if (dir != NULL)
				dumpError("You should use -d or -f\n");
			if (i + 1 >= argc)
				dumpError("Not enough parameters to proceed.\n");
			while ((i + 1) < argc) {
				if (argv[i + 1][0] == '-')
					break;
				addFilename(argv[i + 1]);
				i++;
			}
			continue;
		}

		dumpError("Wrong parameter, use -h for help.");
	}

	/* Loading data from dir */
	if (dir != NULL && nfilelist == 0) {
		fprintf(stderr,
				"Using directory: %s for search for files with ending: %s\n",
				dir, fileend);
		i = ftw(dir, loadfilelist, 20);
		if (i != 0) {
			dumpError("Error loading file list.\n");

		}
		fprintf(stderr, "Discovered %d file%s.\n", nfilelist,
				(nfilelist == 1) ? "" : "s");
		fprintf(stderr, "\n");
	}

	/* Checking for files to process */
	if (nfilelist == 0)
		dumpError("No files, no work.\n");
	fprintf(stderr, "Using %d file%s supplied.\n", nfilelist,
			(nfilelist == 1) ? "" : "s");
	fprintf(stderr, "Copy mode is: %s\n", (copy == 0) ? "OFF" : "ON");

	/* Test default dir */
	if (defaultdir != NULL) {
		while (defaultdir[strlen(defaultdir) - 1] == '/')
			defaultdir[strlen(defaultdir) - 1] = '\0';
		if (!isdir(defaultdir))
			dumpError
				("Default directory doesn't exist or, isn't a directory.\n");

		fprintf(stderr, "Using defaultdir: %s\n", defaultdir);
	}
	fprintf(stderr, "\n");

	/* Generate new names */
	fprintf(stderr, "Generating new names: \n");
	for (j = 0, i = 0; i < nfilelist; i++) {
		j = j + createname(&filelist[i]);
	}
	if (j == 0)
		fprintf(stderr, " All names generated correctly.\n");
	else
		stop = 1;
	fprintf(stderr, "\n");

	/* Test generated names */
	fprintf(stderr, "Testing new names: \n");
	j = checkfilelist();
	if (j == 0)
		fprintf(stderr, " All names tested correctly.\n");
	else
		stop = 1;
	fprintf(stderr, "\n");

	/* Show results */
	if (showme) {
		showfilelist();
		fprintf(stderr, "\n");
	}

	if (stop != 0 && realgo != 1)
		dumpError
			("Will not proceed without --luke flag, errors have been found!\n");


	fprintf(stderr,
			"Stopping before proceed, hit Ctrl-C if you are unsure, proceding in:\n");
	for (i = 5; i > 0; i--) {
		fprintf(stderr, "%ds ", i);
		sleep(1);
	}
	fprintf(stderr, "\n");

	/* Create necessary dirs */
	if (defaultdir != NULL) {
		fprintf(stderr, "Creating directories: \n");
		j = preparefilelistdirs();
		if (j == 0) {
			fprintf(stderr, " All directories created correctly.\n");
			fprintf(stderr, "\n");
		} else
			dumpError(" Problem in directory creation. Bailing out !\n");
	}

	if (stop != 0 && realgo != 1)
		dumpError("Will not proceed without --luke flag !\n");

	/* Rename everybody */
	if (copy == 0) {
		fprintf(stderr, "Moving: \n");
		for (j = 0, i = 0, count = 0; i < nfilelist; i++) {
			if (filelist[i].ready != 1)
				continue;
			k = rename(filelist[i].filename, filelist[i].newfilename);
			j += k;
			if (k != 0) {
				fprintf(stderr, " File %s return an error in moving.\n ",
						filelist[i].filename);
				perror(NULL);
			} else
				count++;
		}

		if (j == 0)
			fprintf(stderr, " %d from %d files renamed correctly.\n",
					count, nfilelist);

	} else {
		fprintf(stderr, "Copying: \n ");
		for (j = 0, i = 0, count = 0; i < nfilelist; i++) {
			if (filelist[i].ready != 1)
				continue;
			k = copyfile(filelist[i].filename, filelist[i].newfilename);
			j += k;
			if (k != 0)
				fprintf(stderr, " File %s return an error in copying.\n ",
						filelist[i].filename);
			else
				count++;
		}

		fprintf(stderr, "\n");
		if (j == 0)
			fprintf(stderr, " %d from %d files renamed correctly.\n",
					count, nfilelist);
	}
	fprintf(stderr, "\n");

	return 0;
}

void showHelp()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr,
			"  sacrename [-h] [-v] [--luke] [--showme] [-c] [-e <fileend>] [-m <dirbase>] <-d <dir>|-f <file 1> ... <file N>>\n\n");

	fprintf(stderr, "\t-h) Show help.\n");
	fprintf(stderr, "\t-v) Show version.\n");
	fprintf(stderr,
			"\t--luke) Use the force luke (do it at your own risk).\n");
	fprintf(stderr, "\t--swhome) Show me what will happen.\n");
	fprintf(stderr, "\t-c) Copy mode, files are copied to new name.\n");
	fprintf(stderr,
			"\t-e) Supply file ending when searching for files.\n");
	fprintf(stderr,
			"\t-m) Change the location of renamed files, imply -c.\n");
	fprintf(stderr, "\t-d) Supply directory for searching for files.\n");
	fprintf(stderr, "\t-f) Supply filenames for process.\n");

}

void showVersion()
{
	extern char version[];

	fprintf(stderr,
			"Version: %s\n         %s\n         %s)\n",
			version, io_version, hd_version);
}

void dumpError(char *string)
{
	fprintf(stderr, "%s\n", string);
	freefilelist();
	exit(-1);
}
