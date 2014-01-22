/*
 * This file is part of sactools package.
 * 
 * sactools is a package to managing and do simple processing with SAC
 * seismological files. Copyright (C) 2012 Marcelo B. de Bianchi
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sachead.h>
#include <timeu.h>

const char *hd_version = "mbLibSac version 0.8 (head)";

static SACHEADDEF x[] = {
	{"delta", offsetof(SACHEAD, delta), 1, 0, 0},
	{"depmin", offsetof(SACHEAD, depmin), 1, 0, 0},
	{"depmax", offsetof(SACHEAD, depmax), 1, 0, 0},
	{"scale", offsetof(SACHEAD, scale), 1, 0, 0},
	{"odelta", offsetof(SACHEAD, odelta), 1, 0, 0},
	{"b", offsetof(SACHEAD, b), 1, 0, 0},
	{"e", offsetof(SACHEAD, e), 1, 0, 0},
	{"o", offsetof(SACHEAD, o), 1, 0, 1},
	{"a", offsetof(SACHEAD, a), 1, 0, 1},
	{"fmt", offsetof(SACHEAD, fmt), 1, 0, 0},
	{"t0", offsetof(SACHEAD, t0), 1, 0, 1},
	{"t1", offsetof(SACHEAD, t1), 1, 0, 1},
	{"t2", offsetof(SACHEAD, t2), 1, 0, 1},
	{"t3", offsetof(SACHEAD, t3), 1, 0, 1},
	{"t4", offsetof(SACHEAD, t4), 1, 0, 1},
	{"t5", offsetof(SACHEAD, t5), 1, 0, 1},
	{"t6", offsetof(SACHEAD, t6), 1, 0, 1},
	{"t7", offsetof(SACHEAD, t7), 1, 0, 1},
	{"t8", offsetof(SACHEAD, t8), 1, 0, 1},
	{"t9", offsetof(SACHEAD, t9), 1, 0, 1},
	{"f", offsetof(SACHEAD, f), 1, 0, 1},
	{"resp0", offsetof(SACHEAD, resp0), 1, 0, 0},
	{"resp1", offsetof(SACHEAD, resp1), 1, 0, 0},
	{"resp2", offsetof(SACHEAD, resp2), 1, 0, 0},
	{"resp3", offsetof(SACHEAD, resp3), 1, 0, 0},
	{"resp4", offsetof(SACHEAD, resp4), 1, 0, 0},
	{"resp5", offsetof(SACHEAD, resp5), 1, 0, 0},
	{"resp6", offsetof(SACHEAD, resp6), 1, 0, 0},
	{"resp7", offsetof(SACHEAD, resp7), 1, 0, 0},
	{"resp8", offsetof(SACHEAD, resp8), 1, 0, 0},
	{"resp9", offsetof(SACHEAD, resp9), 1, 0, 0},
	{"stla", offsetof(SACHEAD, stla), 1, 0, 0},
	{"stlo", offsetof(SACHEAD, stlo), 1, 0, 0},
	{"stel", offsetof(SACHEAD, stel), 1, 0, 0},
	{"stdp", offsetof(SACHEAD, stdp), 1, 0, 0},
	{"evla", offsetof(SACHEAD, evla), 1, 0, 0},
	{"evlo", offsetof(SACHEAD, evlo), 1, 0, 0},
	{"evel", offsetof(SACHEAD, evel), 1, 0, 0},
	{"evdp", offsetof(SACHEAD, evdp), 1, 0, 0},
	{"mag", offsetof(SACHEAD, mag), 1, 0, 0},
	{"user0", offsetof(SACHEAD, user0), 1, 0, 1},
	{"user1", offsetof(SACHEAD, user1), 1, 0, 1},
	{"user2", offsetof(SACHEAD, user2), 1, 0, 1},
	{"user3", offsetof(SACHEAD, user3), 1, 0, 1},
	{"user4", offsetof(SACHEAD, user4), 1, 0, 1},
	{"user5", offsetof(SACHEAD, user5), 1, 0, 1},
	{"user6", offsetof(SACHEAD, user6), 1, 0, 1},
	{"user7", offsetof(SACHEAD, user7), 1, 0, 1},
	{"user8", offsetof(SACHEAD, user8), 1, 0, 1},
	{"user9", offsetof(SACHEAD, user9), 1, 0, 1},
	{"dist", offsetof(SACHEAD, dist), 1, 0, 0},
	{"az", offsetof(SACHEAD, az), 1, 0, 0},
	{"baz", offsetof(SACHEAD, baz), 1, 0, 0},
	{"gcarc", offsetof(SACHEAD, gcarc), 1, 0, 0},
	{"sb", offsetof(SACHEAD, sb), 1, 0, 0},
	{"sdelta", offsetof(SACHEAD, sdelta), 1, 0, 0},
	{"depmen", offsetof(SACHEAD, depmen), 1, 0, 0},
	{"cmpaz", offsetof(SACHEAD, cmpaz), 1, 0, 0},
	{"cmpinc", offsetof(SACHEAD, cmpinc), 1, 0, 0},
	{"xminimum", offsetof(SACHEAD, xminimum), 1, 0, 0},
	{"xmaximum", offsetof(SACHEAD, xmaximum), 1, 0, 0},
	{"yminimum", offsetof(SACHEAD, yminimum), 1, 0, 0},
	{"ymaximum", offsetof(SACHEAD, ymaximum), 1, 0, 0},
	{"unused6", offsetof(SACHEAD, unused6), 1, 0, 0},
	{"unused7", offsetof(SACHEAD, unused7), 1, 0, 0},
	{"unused8", offsetof(SACHEAD, unused8), 1, 0, 0},
	{"unused9", offsetof(SACHEAD, unused9), 1, 0, 0},
	{"unused10", offsetof(SACHEAD, unused10), 1, 0, 0},
	{"unused11", offsetof(SACHEAD, unused11), 1, 0, 0},
	{"unused12", offsetof(SACHEAD, unused12), 1, 0, 0},
	{"nzyear", offsetof(SACHEAD, nzyear), 2, 0, 0},
	{"nzjday", offsetof(SACHEAD, nzjday), 2, 0, 0},
	{"nzhour", offsetof(SACHEAD, nzhour), 2, 0, 0},
	{"nzmin", offsetof(SACHEAD, nzmin), 2, 0, 0},
	{"nzsec", offsetof(SACHEAD, nzsec), 2, 0, 0},
	{"nzmsec", offsetof(SACHEAD, nzmsec), 2, 0, 0},
	{"nvhdr", offsetof(SACHEAD, nvhdr), 2, 0, 0},
	{"norid", offsetof(SACHEAD, norid), 2, 0, 0},
	{"nevid", offsetof(SACHEAD, nevid), 2, 0, 0},
	{"npts", offsetof(SACHEAD, npts), 2, 0, 0},
	{"nsnpts", offsetof(SACHEAD, nsnpts), 2, 0, 0},
	{"nwfid", offsetof(SACHEAD, nwfid), 2, 0, 0},
	{"nxsize", offsetof(SACHEAD, nxsize), 2, 0, 0},
	{"nysize", offsetof(SACHEAD, nysize), 2, 0, 0},
	{"unused15", offsetof(SACHEAD, unused15), 2, 0, 0},
	{"iftype", offsetof(SACHEAD, iftype), 2, 0, 0},
	{"idep", offsetof(SACHEAD, idep), 2, 0, 0},
	{"iztype", offsetof(SACHEAD, iztype), 2, 0, 0},
	{"unused16", offsetof(SACHEAD, unused16), 2, 0, 0},
	{"iinst", offsetof(SACHEAD, iinst), 2, 0, 0},
	{"istreg", offsetof(SACHEAD, istreg), 2, 0, 0},
	{"ievreg", offsetof(SACHEAD, ievreg), 2, 0, 0},
	{"ievtyp", offsetof(SACHEAD, ievtyp), 2, 0, 0},
	{"iqual", offsetof(SACHEAD, iqual), 2, 0, 0},
	{"isynth", offsetof(SACHEAD, isynth), 2, 0, 0},
	{"imagtyp", offsetof(SACHEAD, imagtyp), 2, 0, 0},
	{"imagsrc", offsetof(SACHEAD, imagsrc), 2, 0, 0},
	{"unused19", offsetof(SACHEAD, unused19), 2, 0, 0},
	{"unused20", offsetof(SACHEAD, unused20), 2, 0, 0},
	{"unused21", offsetof(SACHEAD, unused21), 2, 0, 0},
	{"unused22", offsetof(SACHEAD, unused22), 2, 0, 0},
	{"unused23", offsetof(SACHEAD, unused23), 2, 0, 0},
	{"unused24", offsetof(SACHEAD, unused24), 2, 0, 0},
	{"unused25", offsetof(SACHEAD, unused25), 2, 0, 0},
	{"unused26", offsetof(SACHEAD, unused26), 2, 0, 0},
	{"leven", offsetof(SACHEAD, leven), 2, 0, 0},
	{"lpspol", offsetof(SACHEAD, lpspol), 2, 0, 0},
	{"lovrok", offsetof(SACHEAD, lovrok), 2, 0, 0},
	{"lcalda", offsetof(SACHEAD, lcalda), 2, 0, 0},
	{"unused27", offsetof(SACHEAD, unused27), 2, 0, 0},
	{"kstnm", offsetof(SACHEAD, kstnm), 3, 8, 0},
	{"kevnm", offsetof(SACHEAD, kevnm), 3, 16, 0},
	{"khole", offsetof(SACHEAD, khole), 3, 8, 0},
	{"ko", offsetof(SACHEAD, ko), 3, 8, 0},
	{"ka", offsetof(SACHEAD, ka), 3, 8, 0},
	{"kt0", offsetof(SACHEAD, kt0), 3, 8, 0},
	{"kt1", offsetof(SACHEAD, kt1), 3, 8, 0},
	{"kt2", offsetof(SACHEAD, kt2), 3, 8, 0},
	{"kt3", offsetof(SACHEAD, kt3), 3, 8, 0},
	{"kt4", offsetof(SACHEAD, kt4), 3, 8, 0},
	{"kt5", offsetof(SACHEAD, kt5), 3, 8, 0},
	{"kt6", offsetof(SACHEAD, kt6), 3, 8, 0},
	{"kt7", offsetof(SACHEAD, kt7), 3, 8, 0},
	{"kt8", offsetof(SACHEAD, kt8), 3, 8, 0},
	{"kt9", offsetof(SACHEAD, kt9), 3, 8, 0},
	{"kf", offsetof(SACHEAD, kf), 3, 8, 0},
	{"kuser0", offsetof(SACHEAD, kuser0), 3, 8, 0},
	{"kuser1", offsetof(SACHEAD, kuser1), 3, 8, 0},
	{"kuser2", offsetof(SACHEAD, kuser2), 3, 8, 0},
	{"kcmpnm", offsetof(SACHEAD, kcmpnm), 3, 8, 0},
	{"knetwk", offsetof(SACHEAD, knetwk), 3, 8, 0},
	{"kdatrd", offsetof(SACHEAD, kdatrd), 3, 8, 0},
	{"kinst", offsetof(SACHEAD, kinst), 3, 8, 0},
	{"kzdate", 0, TYPE_OTHER, 19, 0},
	{"kztime", 0, TYPE_OTHER, 13, 0}
};

SACHEADDEF *getSacHeadDefFromChar(char *name)
{
	int i, j;
	for (i = 0; i < SAC_HEADER_FIELDS_EXTENDED; i++)
		if ((j = strcasecmp(name, x[i].name)) == 0)
			return &x[i];

	return NULL;
}

int getSacOffsetFromChar(char *name)
{
	SACHEADDEF *p = NULL;

	p = getSacHeadDefFromChar(name);
	if (p != NULL)
		return p->offset;
	else
		return -1;
}

int getSacTypeFromChar(char *name)
{
	SACHEADDEF *p = NULL;

	p = getSacHeadDefFromChar(name);
	if (p != NULL)
		return p->type;
	else
		return -1;
}

int getSacCharsizeFromChar(char *name)
{
	SACHEADDEF *p = NULL;

	p = getSacHeadDefFromChar(name);
	if (p != NULL)
		return p->charsize;
	else
		return -1;
}

int changeCharValueFromChar(SACHEAD * h, char *var, char *value)
{
	SACHEADDEF *s;
	void *p;
	int i;
	char *pvar;

	s = getSacHeadDefFromChar(var);

	if (s == NULL)
		return -1;

	p = &h->delta;
	p = p + s->offset;

	switch (s->type) {
	case (TYPE_FLOAT):
		*(float *) p = atof(value);
		break;

	case (TYPE_INT):
		*(int *) p = atoi(value);
		break;

	case (TYPE_CHAR):
		pvar = (char *) p;
		for (i = 0; i < s->charsize; i++)
			if (i < strlen(value))
				pvar[i] = value[i];
			else
				pvar[i] = ' ';
		break;

	case (TYPE_OTHER):
		return -1;
		break;
	}

	return 0;
}

char *hd_showValueFromChar(SACHEAD * h, char *var, char *fmtf, char *fmti,
						   char *fmts)
{
	SACHEADDEF *s;
	void *p;
	int i, max, min, j;
	char *pvar;
	char *string = NULL;

	char *default_float = "%f";
	char *default_int = "%d";
	char *default_char = "%s";

	if (fmtf == NULL)
		fmtf = default_float;
	if (fmti == NULL)
		fmti = default_int;
	if (fmts == NULL)
		fmts = default_char;

	s = getSacHeadDefFromChar(var);

	if (s == NULL)
		return NULL;

	p = &h->delta;
	p = p + s->offset;

	/*
	 * For sake of sanaty lets make this big -- Shantano fault file. 
	 */
	string = malloc(sizeof(char) * 512);

	switch (s->type) {
	case (TYPE_FLOAT):
		sprintf(string, fmtf, *(float *) p);
		break;

	case (TYPE_INT):
		sprintf(string, fmti, *(int *) p);
		break;

	case (TYPE_CHAR):
		pvar = (char *) p;
		for (i = 0, min = 0; i < s->charsize; i++)
			if (pvar[i] != ' ') {
				min = i;
				break;
			}

		for (i = 0, max = 0; i < s->charsize; i++)
			if (pvar[i] != ' ')
				max = i;

		for (i = min, j = 0; i < max + 1; i++, j++)
			string[j] = pvar[i];

		string[max + 1] = '\0';
		break;

	case (TYPE_OTHER):
		{
			SACTIME *tr = NULL;

			tr = getTimeStructFromSAC(h);

			if (tr) {
				if (strcasecmp(var, "kzdate") == 0) {
					string = print_time(tr, TIME_KZDATE);
				} else if (strcasecmp(var, "kztime") == 0) {
					string = print_time(tr, TIME_KZTIME);
				}
				free(tr);
			}
			tr = NULL;
			break;
		}
	}

	return string;
}

void showHeadElements(char *tab)
{
	int i;
	int perc = 3;

	for (i = 0; i < SAC_HEADER_FIELDS_EXTENDED; i++) {
		fprintf(stderr, "%s%10s type: %3d CharSize: %3d", tab, x[i].name,
				x[i].type, x[i].charsize);
		if ((i + 1) % perc != 0)
			fprintf(stderr, " | ");
		if ((i + 1) % perc == 0)
			fprintf(stderr, "\n");
	}

	fprintf(stderr, "\n");
}

int isMarkField(char *name)
{
	SACHEADDEF *s = getSacHeadDefFromChar(name);

	if (s != NULL) {
		return s->isMark;
	} else {
		return 0;
	}
}
