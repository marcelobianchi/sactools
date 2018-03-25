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
#include <time.h>
#include <string.h>
#include <head.h>
#include <sachead.h>
#include <timeu.h>
#include <math.h>
#include <ctype.h>

int julday(int year, int mon, int day);
int calday(int year, int yearday, int *mon, int *day);

void fixms(SACTIME * value)
{
	if (value->ms > 999) {
		value->fs.tm_sec++;
		value->ms = value->ms - 1000;
		fixms(value);
	} else if (value->ms < 0) {
		value->fs.tm_sec--;
		value->ms = value->ms + 1000;
		fixms(value);
	} else {
		timegm(&value->fs);
		return;
	}
}

void sumation(SACTIME * tr, float n)
{
	int seconds;
	int ms;

	seconds = (int) (n);
	ms = (int) round((n - seconds) * 1000.0);

	tr->fs.tm_sec += seconds;
	tr->ms += ms;

	fixms(tr);

	return;
}

double difference(SACTIME * tr, SACTIME * t)
{
	timegm(&tr->fs);
	timegm(&t->fs);

	time_t ref = timegm(&tr->fs);
	time_t time = timegm(&t->fs);

	return (difftime(time, ref) + (double) (t->ms - tr->ms) / 1000.0 -
			(double) tr->b);
}

char *print_time(SACTIME * t, int format)
{
	char *text;
	int i;
	text = calloc(sizeof(char), 500);

	switch (format) {
	case (TIME_KZDATE):
		strftime(text, 200, "%b %d (%j), %Y", &t->fs);
		for (i = 0; text[i] != '\0'; i++)
			text[i] = toupper(text[i]);
		break;;
	case (TIME_KZTIME):
		strftime(text, 200, "%H:%M:%S", &t->fs);
		sprintf(text, "%s.%03d", text, t->ms);
		break;;
	case (TIME_CHGMT):
		strftime(text, 200, "%Y %j %H %M %S", &t->fs);
		sprintf(text, "%s %03d", text, t->ms);
		break;;
	case (TIME_ISO):
	default:
		strftime(text, 200, "%Y-%m-%dT%H:%M:%S", &t->fs);
		sprintf(text, "%s.%03d", text, t->ms);
		break;
	}

	return text;
}

SACTIME *getTimeStructFromNumbers(int y, int mo, int d, int h, int mi,
								  int s, int ms)
{
	SACTIME *st = NULL;
	int i;

	st = malloc(sizeof(SACTIME));
	st->fs.tm_year = y - 1900;
	st->fs.tm_mon = mo - 1;
	st->fs.tm_mday = d;
	st->fs.tm_hour = h;
	st->fs.tm_min = mi;
	st->fs.tm_sec = s;
	st->ms = ms;
	st->b = 0.0;
	st->fs.tm_isdst = 0;

	// Call MKTIME to update the jday
	//  fprintf(stderr,"%s\n",print_time(st));    
	i = timegm(&st->fs);
	//  fprintf(stderr,"%s\n",print_time(st));

	if (i == -1)
		return NULL;

	return st;
}

SACTIME *getTimeStructFromJDNumbers(int y, int jd, int h, int m, int s,
									int ms)
{
	int mon;
	int day;
	int i;

	i = calday(y, jd, &mon, &day);
	if (i == -1)
		return NULL;

	return getTimeStructFromNumbers(y, mon, day, h, m, s, ms);
}

SACTIME *getTimeStructFromSAC(SACHEAD * h)
{
	SACTIME *st = NULL;

	if (h == NULL)
		return NULL;

	if (h->nzyear == SAC_HEADER_INT_UNDEFINED ||
		h->nzjday == SAC_HEADER_INT_UNDEFINED ||
		h->nzhour == SAC_HEADER_INT_UNDEFINED ||
		h->nzmin == SAC_HEADER_INT_UNDEFINED ||
		h->nzsec == SAC_HEADER_INT_UNDEFINED ||
		h->nzmsec == SAC_HEADER_INT_UNDEFINED) {
		fprintf(stderr, "No time on file trace.");
		return st;
	}

	st = getTimeStructFromJDNumbers(h->nzyear, h->nzjday, h->nzhour,
									h->nzmin, h->nzsec, h->nzmsec);

	if (st == NULL)
		return st;

	st->b = h->b;

	return st;
}

int julday(int year, int mon, int day)
{
	return -1;
}

int calday(int year, int yearday, int *mon, int *day)
{
	int leap;
	int i;
	int pmonth;
	int pday;

	static char daytab[2][13] = {
		{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};

	*mon = -1;
	*day = -1;

	leap = (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));

	if ((yearday < 0) || ((yearday - leap) > 365)) {
		fprintf(stderr, "ERROR Year %d does not have %d days\n", year,
				yearday);
		return -1;
	}

	for (i = 1; yearday > daytab[leap][i]; i++)
		yearday -= daytab[leap][i];

	pmonth = i;
	pday = yearday;

	*day = yearday;
	*mon = i;

	return 0;
}

char *marktime(SACHEAD * h, char *mark, int format)
{
	float val;


	char *text = NULL;

	SACHEADDEF *sd = getSacHeadDefFromChar(mark);
	if (sd == NULL)
		return text;
	if (!sd->isMark)
		return text;

	hdu_getValueFromChar(mark, h, &val, NULL, NULL);
	if (val == SAC_HEADER_FLOAT_UNDEFINED)
		return text;

	SACTIME *t = getTimeStructFromSAC(h);
	sumation(t, val);

	return print_time(t, format);
}
