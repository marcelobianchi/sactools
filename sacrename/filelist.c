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
#include <ctype.h>

#include <filelist.h>
#include <sacio.h>
#include <head.h>
#include <diskwork.h>
#include <libgen.h>


FILES *filelist;				// file list for all program
int nfilelist = 0;
char *fileend = NULL;

int loadfilelist(const char *fpath, const struct stat *sb, int typeflag)
{
	int i = 0;
	extern char *fileend;

	if (fileend == NULL)
		return -1;
	if ((typeflag == FTW_F) &&
		((i =
		  strncasecmp(&fpath[strlen(fpath) - strlen(fileend)], fileend,
					  strlen(fileend))) == 0)
		) {
		addFilename(fpath);
	}

	return 0;
}


void addFilename(const char *fpath)
{
	extern FILES *filelist;
	extern int nfilelist;

	filelist = realloc(filelist, sizeof(FILES) * (nfilelist + 1));
	if (filelist == NULL)
		return;

	filelist[nfilelist].filename = NULL;
	filelist[nfilelist].newfilename = NULL;
	filelist[nfilelist].ready = 0;

	filelist[nfilelist].filename =
		malloc(sizeof(char) * (strlen(fpath) + 1));
	if (filelist[nfilelist].filename == NULL)
		return;
	strcpy(filelist[nfilelist].filename, fpath);

	nfilelist++;
}

void freefilelist()
{
	extern FILES *filelist;
	extern int nfilelist;
	int i;

	for (i = 0; i < nfilelist; i++) {
		io_freeData(filelist[i].filename);
		io_freeData(filelist[i].newfilename);
	}
	io_freeData(filelist);
	nfilelist = 0;
}


void showfilelist()
{
	extern FILES *filelist;
	extern int nfilelist;
	int i, max;
	char fmt[50];

	max = strlen(filelist[0].filename);
	for (i = 1; i < nfilelist; i++)
		if (strlen(filelist[i].filename) > max)
			max = strlen(filelist[i].filename);


	sprintf(fmt, "%%06d\t%%-%ds -> %%s\n", max);
	fprintf(stderr, "Current Filelist: \n\n");
	for (i = 0; i < nfilelist; i++)
		if (filelist[i].ready)
			fprintf(stderr, fmt, i, filelist[i].filename,
					filelist[i].newfilename);
		else
			fprintf(stderr, fmt, i, filelist[i].filename, "NONE");

	return;
}



int checkfilelist()
{
	extern FILES *filelist;
	extern int nfilelist;
	int i, j;
	int flag = 0;

	for (i = 0; i < nfilelist; i++) {
		if (filelist[i].ready == 0)
			continue;

		/* Will rename to same file */
		if (!strcmp(filelist[i].filename, filelist[i].newfilename)) {
			fprintf(stderr,
					" %s will rename to the same file\n",
					filelist[i].filename);
			filelist[i].ready = 0;
			flag = 1;
			continue;
		}

		if (isfile(filelist[i].newfilename)) {
			fprintf(stderr,
					" %s overwrite already existing file.\n",
					filelist[i].filename);
			filelist[i].ready = 0;
			flag = 1;
			continue;
		}

		for (j = i + 1; j < nfilelist; j++) {
			if (filelist[j].ready == 0)
				continue;
			/* New file overwrite a newfilename */
			if (!strcmp(filelist[i].newfilename, filelist[j].newfilename)) {
				fprintf(stderr,
						" %s %s both maps to %s\n",
						filelist[i].filename,
						filelist[j].filename, filelist[j].newfilename);
				filelist[i].ready = 0;
				filelist[j].ready = 0;
				flag = 1;
			}
		}


		/* New filename overwrite an old filename */
		for (j = 0; j < nfilelist; j++) {
			if (!strcmp(filelist[i].newfilename, filelist[j].filename)) {
				fprintf(stderr,
						" %s will rewrite %s\n",
						filelist[i].newfilename, filelist[j].filename);
				filelist[i].ready = 0;
				flag = 1;
			}
		}
	}

	return flag;
}

int preparefilelistdirs()
{
	extern FILES *filelist;
	extern int nfilelist;
	int i, erro;

	erro = 0;
	for (i = 0; i < nfilelist; i++) {
		char *dir = NULL;
		char *temp = NULL;

		if (filelist[i].ready != 1)
			continue;
		temp = copystring(filelist[i].newfilename);
		if (temp != NULL) {
			dir = dirname(temp);
			erro += createdir(dir, 0);
		} else {
			fprintf(stderr, " Error alocating memory.\n");
			erro++;
		}
		io_freeData(temp);
	}

	return erro;
}


int createname(FILES * f)
{
	SACHEAD *h = NULL;
	int i, stop = 0;
	char *temp = NULL;
	char *dir = NULL;
	char *kstnm = NULL;
	char kcmpnm;
	extern char *defaultdir;

	f->ready = 0;

	h = io_readSacHead(f->filename);
	if (h == NULL)
		return -1;

	kstnm = hd_showValueFromChar(h, "kstnm", NULL, NULL, NULL);
	if (kstnm == NULL)
		stop = 1;

	temp = malloc(sizeof(char) * (strlen(f->filename) + 1));
	if (temp == NULL)
		stop = 1;

	/* Tests */
	if (h->nzyear == SAC_HEADER_INT_UNDEFINED) {
		fprintf(stderr, " File %s without year\n", f->filename);
		stop = 1;
	}

	if (h->nzjday == SAC_HEADER_INT_UNDEFINED) {
		fprintf(stderr, " File %s without julian day\n", f->filename);
		stop = 1;
	}

	if (h->nzhour == SAC_HEADER_INT_UNDEFINED) {
		fprintf(stderr, " File %s without hour\n", f->filename);
		stop = 1;
	}

	if (h->nzmin == SAC_HEADER_INT_UNDEFINED) {
		fprintf(stderr, " File %s without minuter\n", f->filename);
		stop = 1;
	}

	if (h->nzsec == SAC_HEADER_INT_UNDEFINED) {
		fprintf(stderr, " File %s without seconds\n", f->filename);
		stop = 1;
	}

	if ((i = strncmp(kstnm, SAC_HEADER_CHAR_UNDEFINED, 6)) == 0) {
		fprintf(stderr, " File %s without stnm\n", f->filename);
		stop = 1;
	}

	if (h->cmpaz == SAC_HEADER_FLOAT_UNDEFINED) {
		fprintf(stderr, " File %s without cmpaz\n", f->filename);
		stop = 1;
	}

	if (h->cmpinc == SAC_HEADER_FLOAT_UNDEFINED) {
		fprintf(stderr, " File %s without cmpinc\n", f->filename);
		stop = 1;
	}

	if (stop) {
		io_freeData(kstnm);
		io_freeData(h);
		io_freeData(temp);
		return -1;
	}

	/* Prepare for dirname */
	strcpy(temp, f->filename);
	dir = dirname(temp);

	/* Lowercase station name */
	for (i = 0; i < strlen(kstnm); i++)
		kstnm[i] = tolower(kstnm[i]);

	if (h->cmpinc == 0.0) {
		kcmpnm = 'z';
	} else if (h->cmpinc == 90.0) {
		if (h->cmpaz == 90)
			kcmpnm = 'e';
		else if (h->cmpaz == 0.0)
			kcmpnm = 'n';
		else
			kcmpnm = 'u';
	} else {
		kcmpnm = 'u';
	}

	/*  dir/YYYY.JJJ.HH.MM.SS.*.Z.sac == 25 + strlen(kstnm) + strlen(dir) + 1 */
	if (defaultdir == NULL) {
		f->newfilename =
			malloc(sizeof(char) * (strlen(dir) + strlen(kstnm) + 25 + 1));
		if (f->newfilename == NULL)
			return -1;
		sprintf(f->newfilename,
				"%s/%04d.%03d.%02d.%02d.%02d.%s.%c.sac",
				dir,
				h->nzyear,
				h->nzjday, h->nzhour, h->nzmin, h->nzsec, kstnm, kcmpnm);
	} else {
		/* mainbasename/YYYY/JJJ/YYYY.JJJ.HH.MM.SS.station.Z.sac == 
		   34 + strlen(station) + strlen(mainbasename) + 1 */
		f->newfilename =
			malloc(sizeof(char) *
				   (strlen(defaultdir) + strlen(kstnm) + 34 + 1));
		if (f->newfilename == NULL)
			return -1;
		sprintf(f->newfilename,
				"%s/%04d/%03d/%04d.%03d.%02d.%02d.%02d.%s.%c.sac",
				defaultdir,
				h->nzyear,
				h->nzjday,
				h->nzyear,
				h->nzjday, h->nzhour, h->nzmin, h->nzsec, kstnm, kcmpnm);
	}

	f->ready = 1;

	io_freeData(temp);
	io_freeData(kstnm);
	io_freeData(h);
	dir = NULL;

	return 0;
}
