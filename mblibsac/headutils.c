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

const char *hdu_version = "mbLibSac version 0.1 (headutils)";

int hdu_changeValueFromChar(SACHEAD * h, char *string, float *Fvalue,
							int *Ivalue, char *Cvalue)
{
	char *pvar;
	void *p;
	int i;
	SACHEADDEF *s;

	s = getSacHeadDefFromChar(string);
	if (s == NULL)
		return -1;

	p = &h->delta;
	p += s->offset;

	switch (s->type) {
	case (TYPE_FLOAT):
		if (Fvalue == NULL)
			return -1;
		*(float *) p = *Fvalue;
		break;

	case (TYPE_INT):
		if (Ivalue == NULL)
			return -1;
		*(int *) p = *Ivalue;
		break;

	case (TYPE_CHAR):
		pvar = (char *) p;
		if (Cvalue == NULL)
			return -1;

		for (i = 0; i < s->charsize; i++)
			if (i < strlen(Cvalue))
				pvar[i] = Cvalue[i];
			else
				pvar[i] = ' ';
		break;
	}

	return 0;
}

int hdu_getValueFromChar(char *string, SACHEAD * hdr, float *Fvalue,
						 int *Ivalue, char **str)
{
	char *pvar;
	int i;
	SACHEADDEF *s;
	void *p;

	s = getSacHeadDefFromChar(string);
	if (s == NULL)
		return -1;

	p = &hdr->delta;
	p += s->offset;

	switch (s->type) {
	case (TYPE_FLOAT):
		if (Fvalue != NULL)
			*Fvalue = *(float *) p;
		break;

	case (TYPE_INT):
		if (Ivalue != NULL)
			*Ivalue = *(int *) p;
		break;

	case (TYPE_CHAR):
		pvar = (char *) (p);
		if (str == NULL)
			break;

		(*str) = malloc(sizeof(char) * s->charsize + 1);

		for (i = 0; i < s->charsize; i++)
			(*str)[i] = pvar[i];
		(*str)[i] = '\0';

		for (i = s->charsize; i >= 0; i--) {
			if ((*str)[i] == ' ' || (*str)[i] == '\0')
				(*str)[i] = '\0';
			else
				break;
		}

		break;
	}

	return s->type;
}

int hdu_getNptsFromSeconds(SACHEAD * h, float picktime)
{
	return (int) (0.50 + ((picktime - h->b) / h->delta));
}

float hdu_getSecondsFromNPTS(SACHEAD * h, int npts)
{
	return (float) (h->delta * npts + h->b);
}

int hdu_roundNPTS(SACHEAD * h, int value)
{
	if (value > h->npts) {
		return h->npts;
	} else if (value >= 0) {
		return value;
	} else {
		return 0;
	}
}

int hdu_checkNPTS(SACHEAD * h, int value)
{
	if (value < 0)
		return 1;
	if (value >= h->npts)
		return 1;
	return 0;
}
