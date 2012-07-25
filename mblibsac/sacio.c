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
#include <stddef.h>

#include <sachead.h>
#include <sacheadEnums.h>

const char *io_version = "mbLibSac version 0.5 (sacio)";

/** 
 * This method is responssible for freeing up memory allocated by io_readSacHead() and io_readSacData() methods.
 * 
 * @param p 
 *  Pointer for the data to be alocated.
 * @return 
 *  Always return NULL to be re-atributed to the pointer.
 */
void *io_freeData(void *p)
{
	if (p != NULL) {
		//    fprintf(stderr,"Liberando . . . ");
		free(p);
	}
	return NULL;
}

/** 
 * Reads the header part of a SAC file. The header can be used later for reading the data part in method io_readSacData()
 * 
 * @param filename Name of file to read
 * 
 * @return A newly SACHEAD data for the given file
 */
SACHEAD *io_readSacHead(char *filename)
{
	FILE *ent;
	int i;
	SACHEAD *h;

	h = NULL;
	h = malloc(sizeof(SACHEAD));
	if (h == NULL)
		return NULL;

	ent = fopen(filename, "r");
	if (ent == NULL)
		return NULL;
	i = fread(h, sizeof(SACHEAD), 1, ent);
	fclose(ent);

	if (i != 1 || h->nvhdr != 6) {
		io_freeData(h);
		return NULL;
	} else {
		return h;
	}
}

/** 
 * Read the data part of the SAC file.
 * 
 * @param filename The name of the SAC file
 * @param h
 *  The header of the SAC file pre-readed with the io_readSacHead().
 * @return 
 *  The newly allocated vector of floating points for the data readed from the SAC file.
 */
float *io_readSacData(char *filename, SACHEAD * h)
{
	FILE *ent;
	float *y = NULL;
	int i, dataspam;

	switch (h->iftype) {
	case (ITIME):
		if (h->leven == SAC_TRUE)
			dataspam = 1;
		else
			dataspam = 2;
		break;

	case (IRLIM):
		dataspam = 2;
		break;

	case (IAMPH):
		dataspam = 2;
		break;

	case (IXY):
		if (h->leven == SAC_TRUE)
			dataspam = 1;
		else
			dataspam = 2;
		break;

	case (IXYZ):
		fprintf(stderr, "Sac file type IXYZ is not supported.");
		return NULL;
		break;
	}

	if (dataspam == 0)
		return NULL;

	y = malloc(sizeof(float) * dataspam * h->npts);
	if (y == NULL)
		return NULL;

	ent = fopen(filename, "r");
	fseek(ent, SAC_HEADER_SIZE, 0);
	i = fread(y, sizeof(float), dataspam * h->npts, ent);
	fclose(ent);

	if (i != dataspam * h->npts) {
		io_freeData(y);
		return NULL;
	} else {
		return y;
	}

}

/** 
 * Read the header and data part in one call. Use this to read the full file to memory.
 * 
 * @param filename The name of the SAC file to read.
 * @param h The newly readed header part of the sac file.
 *  Note: This is an output parameter, you should pass in a pointer to a pointer in this parameter (like &h, where h is of type SACHEAD*).
 * @return The newly readed data part of the sac file.
 */
float *io_readSac(char *filename, SACHEAD ** h)
{
	SACHEAD *ph = NULL;
	float *py = NULL;

	ph = io_readSacHead(filename);
	if (ph == NULL)
		return NULL;

	py = io_readSacData(filename, ph);
	if (py == NULL) {
		io_freeData(ph);
		return NULL;
	}

	(*h) = ph;
	return py;
}

/** 
 * Write the header part to the already existing file.
 * 
 * This code doesn't check the consistency of the data part and header part,
 * you should know what you are doing. Don't mix data from one file, header 
 * from another and write to another one ! 
 * 
 * @param filename The sac filename to write to
 * @param h The header for writting.
 * 
 * @return 0 on success, -1 on error.
 */
int io_writeSacHead(char *filename, SACHEAD * h)
{
	FILE *sai;
	int i;

	if ((sai = fopen(filename, "r+")) == NULL)
		if ((sai = fopen(filename, "w")) == NULL)
			return -1;

	i = fwrite(h, sizeof(SACHEAD), 1, sai);
	fclose(sai);

	if (i == 1)
		return 0;
	else
		return -2;
}

/** 
 * Write the data part to the already existing file.
 * 
 * This code doesn't check the consistency of the data part and header part,
 * you should know what you are doing. Don't mix data from one file, header 
 * from another and write to another one ! 
 *
 * @param filename The filename of the sac file.
 * @param h The header of the sac file.
 * @param y The data part to be written.
 * 
 * @return 0 on success, -1 on error
 */
int io_writeSacData(char *filename, SACHEAD * h, float *y)
{
	FILE *sai;
	int i, dataspam;

	switch (h->iftype) {
	case (ITIME):
		if (h->leven == SAC_TRUE)
			dataspam = 1;
		else
			dataspam = 2;
		break;

	case (IRLIM):
		dataspam = 2;
		break;

	case (IAMPH):
		dataspam = 2;
		break;

	case (IXY):
		if (h->leven == SAC_TRUE)
			dataspam = 1;
		else
			dataspam = 2;
		break;

	case (IXYZ):
		fprintf(stderr, "Sac file type IXYZ is not supported.");
		return -1;
		break;
	}

	if (dataspam == 0)
		return -1;

	if ((sai = fopen(filename, "r+")) == NULL)
		if ((sai = fopen(filename, "w")) == NULL)
			return -1;

	fseek(sai, SAC_HEADER_SIZE, 0);
	i = fwrite(y, sizeof(float), h->npts * dataspam, sai);
	fclose(sai);

	if (i != h->npts * dataspam)
		return -1;
	else
		return 0;
}

/** 
 * Write a new sac file to disk.
 * 
 * @param filename Filename of the file to write
 * @param h The header for the file
 * @param y The data part for the file
 * 
 * @return 0 for success, -1 otherwise.
 */
int io_writeSac(char *filename, SACHEAD * h, float *y)
{
	FILE *sai;
	int i;

	if ((sai = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "No permission to write full new file.\n");
		return -1;
	} else {
		fclose(sai);
	}

	if ((i = io_writeSacHead(filename, h)) != 0)
		return -1;
	if ((i = io_writeSacData(filename, h, y)) != 0)
		return -2;

	return 0;
}
