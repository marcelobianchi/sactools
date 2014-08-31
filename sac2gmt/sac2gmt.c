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
#include <yutils.h>
#include <ftw.h>

char version[] = "sac2gmt version 0.2";
int quiet = 0;

typedef struct fileList {
	char *path;
	float order;
} FILELIST;

typedef struct ConfigModel {
	char *fileend;
	char *sortvar;
	char *refvar;
	char **info;
	int ninfo;

	int useSort;
	float offset;
	int hasoffset;
	int counter;
	int processCounter;

	int normalize;
	int justheader;

	FILELIST *list;
	int nlist;

} CONFIGMODEL;

CONFIGMODEL config;

/* Global vars */

void cleanUp();
static int cmp(const void *p1, const void *p2);
void initConfig(void);
int process(FILELIST * f);
void showHelp();
int preprocess(const char *fpath, const struct stat *sb, int typeflag);
int blacklisted(char *var);
void showVars();
void setOrder(FILELIST * f);

int main(int argc, char **argv)
{
	int i;
	int j;
	int stop;

	/* Files extension used in the search */
	extern CONFIGMODEL config;

	if (argc == 1) {
		showHelp();
		return -1;
	}

	/* Inicialization */
	stop = 0;
	initConfig();
	for (i = 1; i < argc; i++) {
		if ((j = strncmp(argv[i], "-v", 2)) == 0) {
			fprintf(stderr,
					"Version: %s\n         %s\n         %s\n	 %s\n",
					version, io_version, hd_version, hdu_version);
			return 0;
		}

		if ((j = strncmp(argv[i], "-i", 2)) == 0 && i < argc - 1) {
			config.ninfo++;
			config.info =
				realloc(config.info, sizeof(char *) * config.ninfo);
			config.info[config.ninfo - 1] = argv[i + 1];
			continue;
			i++;
		}

		if ((j = strncmp(argv[i], "-u", 2)) == 0) {
			config.useSort = 1;
			continue;
		}

		if ((j = strncmp(argv[i], "-q", 2)) == 0) {
			quiet = 1;
			continue;
		}

		if ((j = strncmp(argv[i], "-n", 2)) == 0) {
			config.normalize = 1;
			continue;
		}

		if ((j = strncmp(argv[i], "-jh", 3)) == 0) {
			config.justheader = 1;
			continue;
		}

		if ((j = strncmp(argv[i], "-r", 2)) == 0 && i < argc - 1) {
			config.refvar = argv[i + 1];
			continue;
			i++;
		}

		if ((j = strncmp(argv[i], "-s", 2)) == 0 && i < argc - 1) {
			config.sortvar = argv[i + 1];
			continue;
			i++;
		}

		if ((j = strncmp(argv[i], "-o", 2)) == 0 && i < argc - 1) {
			config.offset = atof(argv[i + 1]);
			config.hasoffset = 1;
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-d", 2)) == 0 && i < argc - 1) {
			ftw(argv[i + 1], preprocess, 20);
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-e", 2)) == 0 && i < argc - 1) {
			config.fileend = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-f", 2)) == 0 && i < argc - 1) {
			char *path = NULL;
			path = malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
			strcpy(path, argv[i + 1]);
			config.nlist++;
			config.list =
				realloc(config.list, config.nlist * sizeof(FILELIST));
			config.list[config.nlist - 1].path = path;
			i++;
		}
	}

	if (config.refvar != NULL) {
		SACHEADDEF *sd = getSacHeadDefFromChar(config.refvar);
		if (sd == NULL || sd->type != TYPE_FLOAT) {
			fprintf(stderr,
					"Invalid reference variable -- should be of type FLOAT\n");
			stop = 1;
		}
	}

	if (config.sortvar != NULL) {
		SACHEADDEF *sd = getSacHeadDefFromChar(config.sortvar);
		if (sd == NULL || sd->type != TYPE_FLOAT) {
			fprintf(stderr,
					"Invalid sorting variable -- should be of type FLOAT\n");
			stop = 1;
		}
	}

	if (config.ninfo > 0) {
		for (i = 0; i < config.ninfo; i++) {
			SACHEADDEF *sd = getSacHeadDefFromChar(config.info[i]);
			if (sd == NULL) {
				stop = 1;
				fprintf(stderr, "Info var %s not found.\n",
						config.info[i]);
			}
		}
	}

	if (config.nlist == 0) {
		fprintf(stderr, "Error, no file nor directory to process.\n");
		stop = 1;
	}

	if (stop)
		exit(1);


	for (i = 0; i < config.nlist; i++) {
		setOrder(&config.list[i]);
	}
	qsort(config.list, config.nlist, sizeof(FILELIST), cmp);
	for (i = 0; i < config.nlist; i++) {
		process(&config.list[i]);
	}

	return 0;
}

void setOrder(FILELIST * f)
{
	SACHEAD *h = NULL;
	extern CONFIGMODEL config;

	if (config.hasoffset) {
		f->order = (float) config.offset;
		return;
	}

	if (config.sortvar == NULL) {
		f->order = (float) config.counter;
		config.counter++;
		return;
	}

	h = io_readSacHead(f->path);
	if (h == NULL) {
		fprintf(stderr, "Cannot find file %s\n", f->path);
		f->order = -12345.0;
		return;
	}

	float value = -12345.0;
	if (!hdu_getValueFromChar(config.sortvar, h, &value, NULL, NULL)) {
		fprintf(stderr, "Error getting the sortvar from header.");
		f->order = -12345.0;
		return;
	}

	io_freeData(h);

	f->order = value;

	return;
}

int preprocess(const char *fpath, const struct stat *sb, int typeflag)
{
	int i = 0;
	char *path = NULL;
	extern CONFIGMODEL config;

	if ((typeflag == FTW_F) &&
		((i =
		  strncasecmp(&fpath[strlen(fpath) - strlen(config.fileend)],
					  config.fileend, strlen(config.fileend))) == 0)
		) {
		config.nlist++;
		config.list =
			realloc(config.list, config.nlist * sizeof(FILELIST));
		path = malloc((strlen(fpath) + 1) * sizeof(char));
		if (path != NULL) {
			strcpy(path, fpath);
			config.list[config.nlist - 1].path = path;
			path = NULL;
		} else {
			i = 1;
		}
	}

	return 0;
}

int process(FILELIST * f)
{
	SACHEAD *hdr = NULL;
	float *data = NULL;
	float ref = 0.0;
	float value = 0.0;
	int i;

	extern CONFIGMODEL config;


	if (config.useSort) {
		value = f->order;
	} else {
		value = config.processCounter++;
	}

	data = io_readSac(f->path, &hdr);
	if (data == NULL) {
		return 1;
	}

	if (!quiet)
		fprintf(stderr, "Filename: %s Npts: %d Order: %f Value: %f\n",
				f->path, hdr->npts, f->order, value);

	if (config.normalize)
		yu_normalize(data, hdr->npts);

	if (config.refvar) {
		hdu_getValueFromChar(config.refvar, hdr, &ref, NULL, NULL);
	}

	fprintf(stdout, "> %f ", value);

	if (config.ninfo > 0) {
		for (i = 0; i < config.ninfo; i++) {
			if (!isMarkField(config.info[i])) {
				char *string =
					hd_showValueFromChar(hdr, config.info[i], NULL, NULL,
										 NULL);
				if (string == NULL) {
					fprintf(stderr,
							"Variable %s is not defined in file %s",
							config.info[i], f->path);
					fprintf(stdout, "-12345 ");
				} else {
					fprintf(stdout, "%s ", string);
					io_freeData(string);
				}
			} else {
				float mark;
				hdu_getValueFromChar(config.info[i], hdr, &mark, NULL,
									 NULL);
				fprintf(stdout, "%f ", mark - ref);
			}
		}
	}

	fprintf(stdout, "%s\n", f->path);
	if (config.justheader == 0)
		for (i = 0; i < hdr->npts; i++) {
			float time = i * hdr->delta + hdr->b - ref;
			fprintf(stdout, "%f %f %f\n", time, value, data[i]);
		}

	//  offset++;
	io_freeData(data);
	io_freeData(hdr);
	return 0;
}

void showHelp()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr,
			"  sac2gmt [-h] [-v] [-e <file ending string>] <-d <dir> | <-f filename> [-u] [-r <head>] [-s <head>] [-i <head>]\n");

	fprintf(stderr, "\n\t-h) Show this help.\n");
	fprintf(stderr, "\t-v) Show program version.\n");
	fprintf(stderr, "\t-q) Quiet run.\n");
	fprintf(stderr, "\nFile Crawling:\n");
	fprintf(stderr,
			"\t-e) Supply file ending when searching for files.\n");
	fprintf(stderr, "\t-d) Supply directory for searching for files.\n");
	fprintf(stderr, "\t-f) Supply filenames for process.\n");

	fprintf(stderr, "\nOutput Display:\n");
	fprintf(stderr,
			"\t-r) Use header variable as time origin on conversion\n");
	fprintf(stderr, "\t-s) Sort traces by this header variable\n");
	fprintf(stderr,
			"\t-i) Add this variable on the info line, line begging with '>' (use more than one to get more variables in)\n");
	fprintf(stderr, "\t-u) Use sort variable as Second coordinate axis\n");
	fprintf(stderr, "\t-o) Offset on sorting value\n");
	fprintf(stderr, "\t-n) Normalize amplitudes to 1 before output\n");
	fprintf(stderr, "\t-jh) Just print headers lines\n");
}

void initConfig(void)
{
	char *defaultfileend = ".sac";
	extern CONFIGMODEL config;
	config.fileend = defaultfileend;
	config.sortvar = NULL;
	config.refvar = NULL;
	config.info = NULL;
	config.ninfo = 0;
	config.offset = 0;
	config.hasoffset = 0;
	config.normalize = 0;
	config.counter = 1;
	config.processCounter = 1;
	config.list = NULL;
	config.nlist = 0;
	config.useSort = 0;
	config.justheader = 0;
}

static int cmp(const void *p1, const void *p2)
{
	FILELIST *pp1 = (FILELIST *) p1;
	FILELIST *pp2 = (FILELIST *) p2;
	if (pp1->order > pp2->order) {
		return 1;
	} else if (pp1->order < pp2->order) {
		return -1;
	} else {
		return 0;
	}
}

void cleanUp()
{
	extern CONFIGMODEL config;
	int i;

	if (config.info != NULL)
		free(config.info);
	config.info = NULL;
	config.ninfo = 0;

	if (config.list != NULL) {
		for (i = 0; i < config.nlist; i++) {
			free(config.list[i].path);
			config.list[i].path = NULL;
		}
	}
	free(config.list);
	config.list = NULL;
	config.nlist = 0;
}
